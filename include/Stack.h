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

#pragma once
#ifndef SHARE_STACK_H
#define SHARE_STACK_H

#include <cstddef>
#include <cstdint>

namespace CO {
class Routine;
class Stack {
   protected:
    Routine* routine = nullptr;
    std::uint8_t* stack_data = nullptr;
    std::size_t stack_size;
    friend class Runtime;
    friend class Routine;

   public:
    Stack(std::size_t stack_size);
    ~Stack();

    std::uint8_t* bp();
};

}  // namespace CO

#endif
