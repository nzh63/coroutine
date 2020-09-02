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
#ifndef RUNTIME_H
#define RUNTIME_H

#include <forward_list>

#include "Routine.h"
#include "Stack.h"
#include "StackPool.h"

namespace CO {
class Runtime {
   public:
    // typedef void (*SpawnFunction)();
    typedef Routine::SpawnFunction SpawnFunction;

   protected:
    std::forward_list<Routine> routines{Routine()};
    Routine* current;
    std::forward_list<Routine>::iterator last_routines, rr_ptr;
    Runtime();
    Routine* mainRoutine();
    void swichTo(Routine* next);
    std::forward_list<Routine>::iterator newRoutine();
    void _spawn(SpawnFunction f, std::forward_list<Routine>::iterator routine);
    static void _spawn_start();

   public:
    static Runtime* instance();

    bool yield();
    void guard();
    void run();
    void spawn(SpawnFunction f, std::size_t stack_size = 128 * 1024);
    void spawn(SpawnFunction f, Stack* share_stack);
    void spawn(SpawnFunction f, StackPool* share_stack_pool);
};
}  // namespace CO

#endif