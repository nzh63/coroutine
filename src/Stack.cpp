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

#include "Stack.h"

namespace co {
Stack::Stack(std::size_t stack_size) : stack_size_(stack_size) {
    this->stack_data_ = new std::uint8_t[this->stack_size_];
}
Stack::~Stack() { delete[] this->stack_data_; }

std::uint8_t* Stack::bp() { return this->stack_data_ + this->stack_size_; }
}  // namespace co
