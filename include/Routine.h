/*
 * Copyright 2020-present nzh63
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

#pragma once
#ifndef ROUTINE_H
#define ROUTINE_H

#include <array>
#include <cstdint>
#include <functional>
#include <list>

#include "Stack.h"

namespace co {
class Runtime;
class Routine {
   protected:
    typedef std::function<void()> SpawnFunction;
    enum class State : int {
        Idle,    // 空闲，可分配任务
        Peding,  // 等待其他协程返回数据
        Ready,   // 已分配任务，可运行
        Running  // 运行中
    };
    struct Context {
#if defined(CO_ARCH_RISCV)
        std::uint64_t x1, x2, x8, x9, x18, x19, x20, x21, x22, x23, x24, x25,
            x26, x27;
        std::uint32_t f8, f9, f18_f27[10];
        std::uint64_t jump_to;
#elif defined(CO_ARCH_AARCH64)
        std::uint64_t x18_x28[11], fp, lr, sp, jump_to;
        std::uint64_t d8_d15[8];
#elif defined(CO_ARCH_MIPS64)
        std::uint64_t s0_s7[8], gp, sp, s8, ra, jump_to;
        std::uint64_t f20, f22, f24_31[8];
#elif defined(CO_ARCH_x64)
        std::uint64_t rsp, r12, r13, r14, r15, rbx, rbp;
#if defined(CO_WINDOWS)
        std::uint64_t rdi, rsi, stack_start, stack_end;
        std::uint64_t xmm6[2], xmm7[2], xmm8[2], xmm9[2], xmm10[2], xmm11[2],
            xmm12[2], xmm13[2], xmm14[2], xmm15[2];
#endif
#elif defined(CO_ARCH_x86)
        std::uint32_t ebx, ecx, edx, esi, edi, esp, edp;
#if defined(CO_WINDOWS)
        std::uint32_t stack_start, stack_end, exception_registartion;
#endif
#endif
    };

   public:
    explicit Routine();  // main routine
    explicit Routine(SpawnFunction f, Stack* share_stack);
    explicit Routine(SpawnFunction f, std::size_t stack_size);
    ~Routine();

   public:
    static Routine* current();

   public:
    void defer(SpawnFunction callback);

   protected:
    static void entry();

   protected:
    void initCtx();
    void saveStack();
    void saveStack(std::uint8_t* sp);
    void loadStack();
    void runDefers();

   protected:
    friend class co::Runtime;
    bool share_stack_ = false;
    Stack* stack_ = nullptr;
    std::uint8_t* save_buffer_ = nullptr;
    std::size_t save_size_ = 0;
    volatile State state_ = State::Idle;
    void* return_value_ = nullptr;
    SpawnFunction f_ = nullptr;
    std::list<SpawnFunction> defers_;
    Context ctx_;
};
}  // namespace co

#endif  // ROUTINE_H
