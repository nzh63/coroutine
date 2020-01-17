#pragma once
#ifndef COROUTINE_H
#define COROUTINE_H

#include <array>
#include <cstdint>
#include <vector>

namespace CO {

constexpr unsigned STACK_SIZE = 1024 * 1024 * 2;
class Coroutine;
class Runtime;

void guard();
class Coroutine {
   protected:
    struct Context {
       public:
        std::uint64_t
#ifdef __riscv
            x1,
            x2, x8, x9, x18, x19, x20, x21, x22, x23, x24, x25, x26, x27
#else
#ifdef WIN32
            xmm6[2],
            xmm7[2], xmm8[2], xmm9[2], xmm10[2], xmm11[2], xmm12[2], xmm13[2],
            xmm14[2], xmm15[2],
#endif
            rsp, r12, r13, r14, r15, rbx, rbp
#ifdef WIN32
            ,
            rdi, rsi, stack_start, stack_end
#endif
#endif
            ;
#ifdef __riscv
        std::uint32_t f8, f9, f18_f27[10];
        std::uint64_t jump_to;
#endif
    };
    unsigned id;
    std::array<std::uint8_t, STACK_SIZE> stack;
    Context ctx;
    enum class State { Idle, Peding, Ready, Running };
    State state = State::Idle;
    friend class Runtime;

   public:
    Coroutine() = default;
    Coroutine(unsigned id);
};
class Runtime {
   protected:
    std::vector<Coroutine *> coroutines{new Coroutine(0)};
    unsigned current = 0;

   public:
    typedef void (*SpawnFunction)();
    Runtime();
    bool yield(Coroutine::State state = Coroutine::State::Ready);
    void swichTo(unsigned pos);
    void run();
    void c_return();
    void spawn(SpawnFunction f);
};
extern Runtime *g_runtime;
}  // namespace CO
#endif
