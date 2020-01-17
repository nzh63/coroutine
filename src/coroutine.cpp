#include "coroutine.h"

#include <cstdlib>

#ifndef WIN32
extern "C" void _doSwichTo(void* save, void* load) __attribute__((noinline));
#else
extern "C" __declspec(noinline) void __stdcall _doSwichTo(void* save,
                                                          void* load);
#endif

namespace CO {
Runtime* g_runtime;
void guard() { g_runtime->c_return(); }

Coroutine::Coroutine(unsigned id) : id(id){};
Runtime::Runtime() {
    g_runtime = this;
    this->coroutines[0]->state = Coroutine::State::Running;
}

bool Runtime::yield(Coroutine::State state) {
    auto pos = (this->current + 1) % this->coroutines.size();
    while (this->coroutines[pos]->state != Coroutine::State::Ready) {
        pos++;
        if (pos == this->coroutines.size()) pos = 0;
        if (pos == (this->current) % this->coroutines.size()) return false;
    }
    if (this->coroutines[this->current]->state != Coroutine::State::Idle)
        this->coroutines[this->current]->state = state;
    this->coroutines[pos]->state = Coroutine::State::Running;
    this->swichTo(pos);
    return true;
}
void Runtime::swichTo(unsigned pos) {
    auto old_pos = this->current;
    this->current = pos;
    auto save = &this->coroutines[old_pos]->ctx;
    auto load = &this->coroutines[pos]->ctx;
    _doSwichTo((void*)save, (void*)load);
    return;
}
void Runtime::run() {
    while (this->yield())
        ;
}

void Runtime::c_return() {
    if (this->current != 0) {
        this->coroutines[this->current]->state = Coroutine::State::Idle;
        this->yield(Coroutine::State::Idle);
    }
}
void Runtime::spawn(SpawnFunction f) {
    unsigned pos = 0;
    for (int i = 0; i < this->coroutines.size(); i++) {
        if (this->coroutines[i]->state == Coroutine::State::Idle) {
            pos = i;
            break;
        }
        if (i == this->coroutines.size() - 1) {
            pos = this->coroutines.size();
            this->coroutines.push_back(new Coroutine(this->coroutines.size()));
            break;
        }
    }
    this->coroutines[pos]->state = Coroutine::State::Ready;
    std::uint8_t* s_ptr = this->coroutines[pos]->stack.data() +
                          this->coroutines[pos]->stack.size();
#ifdef __riscv
    this->coroutines[pos]->ctx.x1 = (std::uint64_t)(void*)guard;
    this->coroutines[pos]->ctx.jump_to = (std::uint64_t)(void*)f;
    this->coroutines[pos]->ctx.x2 = (std::uint64_t)(void*)(s_ptr - 32);
#else
    *(std::uint64_t*)(s_ptr - 24) = (std::uint64_t)(void*)guard;
    *(std::uint64_t*)(s_ptr - 32) = (std::uint64_t)(void*)f;
    this->coroutines[pos]->ctx.rsp = (std::uint64_t)(void*)(s_ptr - 32);
#ifdef WIN32
    this->coroutines[pos]->ctx.stack_start = (std::uint64_t)(void*)s_ptr;
    this->coroutines[pos]->ctx.stack_end =
        (std::uint64_t)(void*)(s_ptr - STACK_SIZE);
#endif
#endif
}

}  // namespace CO
