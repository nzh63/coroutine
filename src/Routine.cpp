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

#include "Routine.h"

#include <cassert>
#include <cstring>

namespace CO {
Routine::Routine(Stack* stack, bool share_stack)
    : stack(stack), share_stack(share_stack) {}

Routine::~Routine() {}

void Routine::saveStack() {
    assert(this->share_stack);
    std::uint8_t* sp = (std::uint8_t*)(
#if defined(ARCH_RISCV)
        this->ctx.x2
#elif defined(ARCH_x64)
        this->ctx.rsp
#else
        this->ctx.sp
#endif
    );
    std::size_t len = this->stack->bp() - sp;
    assert(this->save_buffer == nullptr);
    this->save_buffer = new std::uint8_t[len];
    this->save_size = len;
    std::memcpy(this->save_buffer, this->stack->bp() - len, len);
    this->stack->routine = nullptr;
}

void Routine::saveStack(std::uint8_t* sp) {
    std::size_t len = this->stack->bp() - sp;
    assert(this->save_buffer == nullptr);
    this->save_buffer = new std::uint8_t[len];
    this->save_size = len;
    std::memcpy(this->save_buffer, this->stack->bp() - len, len);
    this->stack->routine = nullptr;
}

void Routine::loadStack() {
    assert(this->stack->routine == nullptr);
    assert(this->save_buffer != nullptr || this->save_size == 0);
    if (this->save_size != 0) {
        auto len = this->save_size;
        std::memcpy(this->stack->bp() - len, this->save_buffer, len);
        delete[] this->save_buffer;
        this->save_buffer = nullptr;
        this->save_size = 0;
    }
    this->stack->routine = this;
}
}  // namespace CO