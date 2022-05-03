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

#include "StackPool.h"

namespace co {
StackPool::StackPool(std::size_t size, std::size_t count) : count_(count) {
    this->stacks_ = new Stack*[this->count_];
    for (std::size_t i = 0; i < this->count_; i++) {
        this->stacks_[i] = new Stack(size);
    }
}

StackPool::~StackPool() {
    for (std::size_t i = 0; i < this->count_; i++) {
        delete this->stacks_[i];
    }
    delete[] this->stacks_;
}

Stack* StackPool::next() {
    auto* ret = this->stacks_[this->rr_ptr_];
    this->rr_ptr_++;
    if (this->rr_ptr_ == this->count_) this->rr_ptr_ = 0;
    return ret;
}

}  // namespace co
