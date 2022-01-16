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

#include "Runtime.h"

#if defined(CO_ARCH_MIPS64)
extern "C" std::uint64_t _getGP() __attribute__((noinline));
extern "C" std::uint64_t _mips_guard() __attribute__((noinline));
#endif

namespace co {

Routine::Routine()
    : f_(nullptr), share_stack_(false), state_(Routine::State::Running) {}

Routine::Routine(Routine::SpawnFunction f, Stack* stack)
    : f_(f), state_(Routine::State::Ready) {
    if (!this->share_stack_ && this->stack_) delete this->stack_;
    this->stack_ = stack;
    this->share_stack_ = true;
    this->stack_->routine_ = this;
    initCtx();
}

Routine::Routine(Routine::SpawnFunction f, std::size_t stack_size)
    : f_(f), state_(Routine::State::Ready) {
    if (this->share_stack_ || !this->stack_ ||
        this->stack_->stack_size_ != stack_size) {
        if (!this->share_stack_ && this->stack_) delete this->stack_;
        this->stack_ = new Stack(stack_size);
    }
    this->stack_->routine_ = this;
    this->share_stack_ = false;
    initCtx();
}

Routine::~Routine() {
    this->runDefers();
    if (this->share_stack_) {
        this->stack_->routine_ = nullptr;
        this->stack_ = nullptr;
        this->share_stack_ = false;
    }
    f_ = nullptr;
    state_ = State::Idle;
}

Routine* Routine::current() { return Runtime::instance()->current_; }

void Routine::defer(Routine::SpawnFunction callback) {
    this->defers_.emplace_back(std::move(callback));
    if (this->state_ == State::Peding) {
        this->state_ = State::Ready;
    }
}

void Routine::initCtx() {
    auto* guard = +[]() { Runtime::instance()->guard(); };
    std::uint8_t* s_ptr = this->stack_->bp();
    s_ptr -= (std::uint64_t)s_ptr % 16;
#if defined(CO_ARCH_RISCV)
    this->ctx_.x1 = (std::uint64_t)(void*)guard;
    this->ctx_.jump_to = (std::uint64_t)(void*)entry;
    this->ctx_.x2 = (std::uint64_t)(void*)(s_ptr - 0);
#elif defined(CO_ARCH_AARCH64)
    this->ctx_.sp = (std::uint64_t)(void*)(s_ptr - 16);  // Red zone
    this->ctx_.fp = (std::uint64_t)(void*)(s_ptr - 0);
    this->ctx_.lr = (std::uint64_t)(void*)guard;
    this->ctx_.jump_to = (std::uint64_t)(void*)entry;
#elif defined(CO_ARCH_MIPS64)
    this->ctx_.sp = (std::uint64_t)(void*)(s_ptr - 8);
    *(std::uint64_t*)(s_ptr - 8) = (std::uint64_t)(void*)guard;
    this->ctx_.s8 = (std::uint64_t)(void*)(s_ptr - 0);  // fp
    this->ctx_.gp = _getGP();
    this->ctx_.ra = (std::uint64_t)(void*)_mips_guard;
    this->ctx_.jump_to = (std::uint64_t)(void*)entry;
#elif defined(CO_ARCH_x64)
    *(std::uint64_t*)(s_ptr - 8) = (std::uint64_t)(void*)guard;
    *(std::uint64_t*)(s_ptr - 16) = (std::uint64_t)(void*)entry;
    this->ctx_.rsp = (std::uint64_t)(void*)(s_ptr - 16);
#ifdef CO_WINDOWS
    this->ctx_.stack_start = (std::uint64_t)(void*)s_ptr;
    this->ctx_.stack_end = (std::uint64_t)(void*)(this->stack_->stack_data_);
#endif
#elif defined(CO_ARCH_x86)
    *(std::uint32_t*)(s_ptr - 4) = (std::uint32_t)(void*)guard;
#ifdef CO_WINDOWS
    this->ctx_.stack_start = (std::uint32_t)(void*)s_ptr;
    this->ctx_.stack_end = (std::uint32_t)(void*)(this->stack_->stack_data_);
    this->ctx_.exception_registartion = 0xffffffff;
#endif
#if defined(_MSC_VER)
    *(std::uint32_t*)(s_ptr - 16) = (std::uint32_t)(void*)entry;
    this->ctx_.esp = (std::uint32_t)(void*)(s_ptr - 16);
#else
    *(std::uint32_t*)(s_ptr - 8) = (std::uint32_t)(void*)entry;
    this->ctx_.esp = (std::uint32_t)(void*)(s_ptr - 8);
#endif
#endif
}

void Routine::saveStack() {
    assert(this->share_stack_);
    std::uint8_t* sp = (std::uint8_t*)(
#if defined(CO_ARCH_RISCV)
        this->ctx_.x2
#elif defined(CO_ARCH_x64)
        this->ctx_.rsp
#elif defined(CO_ARCH_x86)
        this->ctx_.esp
#else
        this->ctx_.sp
#endif
    );
    std::size_t len = this->stack_->bp() - sp;
    assert(this->save_buffer_ == nullptr);
    this->save_buffer_ = new std::uint8_t[len];
    this->save_size_ = len;
    std::memcpy(this->save_buffer_, this->stack_->bp() - len, len);
    this->stack_->routine_ = nullptr;
}

void Routine::saveStack(std::uint8_t* sp) {
    std::size_t len = this->stack_->bp() - sp;
    assert(this->save_buffer_ == nullptr);
    this->save_buffer_ = new std::uint8_t[len];
    this->save_size_ = len;
    std::memcpy(this->save_buffer_, this->stack_->bp() - len, len);
    this->stack_->routine_ = nullptr;
}

void Routine::loadStack() {
    assert(this->stack_->routine_ == nullptr);
    assert(this->save_buffer_ != nullptr || this->save_size_ == 0);
    if (this->save_size_ != 0) {
        auto len = this->save_size_;
        std::memcpy(this->stack_->bp() - len, this->save_buffer_, len);
        delete[] this->save_buffer_;
        this->save_buffer_ = nullptr;
        this->save_size_ = 0;
    }
    this->stack_->routine_ = this;
}

void Routine::runDefers() {
    while (!this->defers_.empty()) {
        auto f = std::move(this->defers_.front());
        this->defers_.pop_front();
        f();
    }
}

void Routine::entry() {
    auto* runtime = Runtime::instance();
    auto& f = runtime->current_->f_;
    f();
}
}  // namespace co
