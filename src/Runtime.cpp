/*
 * Copyright 2020 nzh63
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Runtime.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#if defined(_MSC_VER)
extern "C" __declspec(noinline) void __stdcall _doSwichTo(void* save,
                                                          void* load);
#else
extern "C" void _doSwichTo(void* save, void* load) __attribute__((noinline));
#if defined(ARCH_MIPS64)
extern "C" std::uint64_t _getGP() __attribute__((noinline));
extern "C" std::uint64_t _mips_guard() __attribute__((noinline));
#endif
#endif

namespace CO {
namespace {
void guard() { Runtime::instance()->guard(); }
}  // namespace

Runtime::Runtime() {
    this->current = this->mainRoutine();
    this->rr_ptr = this->last_routines = this->routines.begin();
    this->current->state = Routine::State::Running;
}

Routine* Runtime::mainRoutine() { return &this->routines.front(); }

Runtime* Runtime::instance() {
    static Runtime* _runtime = nullptr;
    if (!_runtime) _runtime = new Runtime();
    return _runtime;
}

bool Runtime::yield() {
    assert(this->current->state == Routine::State::Running ||
           this->current->state == Routine::State::Idle ||
           this->current == this->mainRoutine());
    Routine* next = nullptr;
    {
        auto nextIt = std::next(this->rr_ptr);
        if (nextIt == this->routines.end()) nextIt = this->routines.begin();
        while (nextIt->state != Routine::State::Ready) {
            if (nextIt == this->rr_ptr) return false;
            nextIt++;
            if (nextIt == this->routines.end()) nextIt = this->routines.begin();
        }
        next = &*nextIt;
        if (!(this->current->share_stack && next->share_stack &&
              this->current->stack == next->stack)) {
            this->rr_ptr = nextIt;
        }
    }
    if (this->current->share_stack && next->share_stack &&
        this->current->stack == next->stack) {
        this->swichTo(this->mainRoutine());
        return true;
    }
    if (this->current->state != Routine::State::Idle)
        this->current->state = Routine::State::Ready;
    next->state = Routine::State::Running;
    this->swichTo(&*next);
    return true;
}
void Runtime::swichTo(Routine* next) {
    if (this->current == next) return;
    auto save = &this->current->ctx;
    auto load = &next->ctx;
    assert(next->stack != this->current->stack);
    if (next->share_stack && next->stack->routine != &*next) {
        auto* r = next->stack->routine;
        if (r) {
            r->saveStack();
        }
        next->loadStack();
    }
    this->current = next;
    _doSwichTo((void*)save, (void*)load);
    assert(this->current != next);
    return;
}
void Runtime::run() {
    while (this->yield())
        ;
}

void Runtime::guard() {
    if (this->current != this->mainRoutine()) {
        this->current->state = Routine::State::Idle;
        if (this->current->share_stack) {
            this->current->stack->routine = nullptr;
        }
        this->yield();
    }
}
std::forward_list<Routine>::iterator Runtime::newRoutine() {
    auto routine = this->routines.end();
    for (auto it = this->routines.begin(); it != this->routines.end(); it++) {
        if (it->state == Routine::State::Idle) {
            routine = it;
            break;
        }
    }
    if (routine == this->routines.end()) {
        routine = this->routines.emplace_after(this->last_routines);
        this->last_routines = routine;
    }
    return routine;
}

void Runtime::spawn(SpawnFunction f, std::size_t stack_size) {
    auto routine = this->newRoutine();
    if (routine->share_stack) {
        routine->share_stack = false;
        routine->stack = nullptr;
    }
    if (routine->stack && routine->stack->stack_size != stack_size) {
        delete routine->stack;
        routine->stack = new Stack(stack_size);
    }
    if (!routine->stack) {
        routine->stack = new Stack(stack_size);
    }
    routine->stack->routine = &*routine;
    routine->share_stack = false;
    routine->state = Routine::State::Ready;
    _spawn(f, routine);
}

void Runtime::spawn(SpawnFunction f, Stack* share_stack) {
    auto routine = this->newRoutine();
    if (routine->stack && !routine->share_stack) {
        delete routine->stack;
    }
    if (share_stack->routine != nullptr) {
        share_stack->routine->saveStack();
    }
    routine->stack = share_stack;
    routine->stack->routine = &*routine;
    routine->share_stack = true;
    routine->state = Routine::State::Ready;
    _spawn(f, routine);
}

void Runtime::spawn(SpawnFunction f, StackPool* share_stack_pool) {
    spawn(f, share_stack_pool->stacks[share_stack_pool->rr_ptr]);
    share_stack_pool->rr_ptr++;
    if (share_stack_pool->rr_ptr == share_stack_pool->count)
        share_stack_pool->rr_ptr = 0;
}

void Runtime::_spawn(SpawnFunction f,
                     std::forward_list<Routine>::iterator routine) {
    std::uint8_t* s_ptr = routine->stack->bp();
    s_ptr -= (std::uint64_t)s_ptr % 16;
#if defined(ARCH_RISCV)
    routine->ctx.x1 = (std::uint64_t)(void*)CO::guard;
    routine->ctx.jump_to = (std::uint64_t)(void*)CO::Runtime::_spawn_start;
    routine->ctx.x2 = (std::uint64_t)(void*)(s_ptr - 0);
#elif defined(ARCH_AARCH64)
    routine->ctx.sp = (std::uint64_t)(void*)(s_ptr - 16);  // Red zone
    routine->ctx.fp = (std::uint64_t)(void*)(s_ptr - 0);
    routine->ctx.lr = (std::uint64_t)(void*)CO::guard;
    routine->ctx.jump_to = (std::uint64_t)(void*)CO::Runtime::_spawn_start;
#elif defined(ARCH_MIPS64)
    routine->ctx.sp = (std::uint64_t)(void*)(s_ptr - 8);
    *(std::uint64_t*)(s_ptr - 8) = (std::uint64_t)(void*)CO::guard;
    routine->ctx.s8 = (std::uint64_t)(void*)(s_ptr - 0);  // fp
    routine->ctx.gp = _getGP();
    routine->ctx.ra = (std::uint64_t)(void*)_mips_guard;
    routine->ctx.jump_to = (std::uint64_t)(void*)CO::Runtime::_spawn_start;
#elif defined(ARCH_x64)
    *(std::uint64_t*)(s_ptr - 8) = (std::uint64_t)(void*)CO::guard;
    *(std::uint64_t*)(s_ptr - 16) =
        (std::uint64_t)(void*)CO::Runtime::_spawn_start;
    routine->ctx.rsp = (std::uint64_t)(void*)(s_ptr - 16);
#ifdef WIN32
    routine->ctx.stack_start = (std::uint64_t)(void*)s_ptr;
    routine->ctx.stack_end = (std::uint64_t)(void*)(routine->stack->stack_data);
#endif
#endif
    routine->f = f;
}

void Runtime::_spawn_start() {
    auto* runtime = Runtime::instance();
    auto f = runtime->current->f;
    f();
}

}  // namespace CO