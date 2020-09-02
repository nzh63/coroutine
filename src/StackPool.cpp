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

#include "StackPool.h"

namespace CO {
StackPool::StackPool(std::size_t size, std::size_t count) : count(count) {
    this->stacks = new Stack*[this->count];
    for (std::size_t i = 0; i < this->count; i++) {
        this->stacks[i] = new Stack(size);
    }
}

StackPool::~StackPool() {
    for (std::size_t i = 0; i < this->count; i++) {
        delete this->stacks[i];
    }
    delete[] this->stacks;
}
}  // namespace CO
