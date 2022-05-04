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
#endif

namespace co {

Runtime::Runtime() {
    this->routines_.emplace_front();
    this->current_ = this->mainRoutine();
    this->current_->state_ = Routine::State::Running;
    this->rr_ptr_ = this->last_routines_ = this->routines_.begin();
}

Routine* Runtime::mainRoutine() { return &this->routines_.front(); }

Runtime* Runtime::instance() {
    static Runtime* _runtime = nullptr;
    if (!_runtime) _runtime = new Runtime();
    return _runtime;
}

bool Runtime::yield(bool await_for_other) {
    assert(this->current_->state_ == Routine::State::Running ||
           this->current_->state_ == Routine::State::Idle ||
           this->current_ == this->mainRoutine());
    Routine* next = nullptr;
    {
        auto nextIt = std::next(this->rr_ptr_);
        if (nextIt == this->routines_.end()) nextIt = this->routines_.begin();
        while (nextIt->state_ != Routine::State::Ready) {
            if (nextIt == this->rr_ptr_) return false;
            nextIt++;
            if (nextIt == this->routines_.end())
                nextIt = this->routines_.begin();
        }
        next = &*nextIt;
        if (!(this->current_->share_stack_ && next->share_stack_ &&
              this->current_->stack_ == next->stack_)) {
            this->rr_ptr_ = nextIt;
        }
    }

    if (this->current_->share_stack_ && next->share_stack_ &&
        this->current_->stack_ == next->stack_) {
        this->swichTo(this->mainRoutine());
        return true;
    }
    if (this->current_->state_ == Routine::State::Running)
        this->current_->state_ =
            await_for_other ? Routine::State::Peding : Routine::State::Ready;
    next->state_ = Routine::State::Running;
    this->swichTo(next);
    return true;
}

void Runtime::swichTo(Routine* next) {
    if (this->current_ == next) return;
    assert(this->current_->stack_ == nullptr ||
           this->current_->stack_ != next->stack_);
    auto* save = &this->current_->ctx_;
    auto* load = &next->ctx_;
    if (next->share_stack_ && next->stack_->routine_ != &*next) {
        auto* r = next->stack_->routine_;
        if (r) {
            r->saveStack();
        }
        next->loadStack();
    }
    this->current_ = next;
    _doSwichTo((void*)save, (void*)load);
    assert(this->current_ != next);
    this->current_->runDefers();
}

void Runtime::run() {
    while (this->yield(true))
        ;
}

void Runtime::guard() {
    assert(this->current_ != this->mainRoutine());
    this->current_->state_ = Routine::State::Idle;
    this->mainRoutine()->state_ = Routine::State::Ready;
    this->current_->~Routine();
    this->yield();
}

void Runtime::spawn(Runtime::SpawnFunction f, std::size_t stack_size) {
    auto* routine = &*this->newRoutine();
    new (routine) Routine(f, stack_size);
}

void Runtime::spawn(Runtime::SpawnFunction f, Stack* share_stack) {
    auto* routine = &*this->newRoutine();
    new (routine) Routine(f, share_stack);
}

void Runtime::spawn(Runtime::SpawnFunction f, StackPool* share_stack_pool) {
    spawn(f, share_stack_pool->next());
}

std::forward_list<Routine>::iterator Runtime::newRoutine() {
    auto routine = this->routines_.end();
    for (auto it = this->routines_.begin(); it != this->routines_.end(); it++) {
        if (it->state_ == Routine::State::Idle) {
            routine = it;
            break;
        }
    }
    if (routine == this->routines_.end()) {
        routine = this->routines_.emplace_after(this->last_routines_);
        this->last_routines_ = routine;
    }
    return routine;
}

}  // namespace co
