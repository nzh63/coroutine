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

#include <iostream>

#include "coroutine.h"

using namespace CO;

int main() {
    std::cout << "Hello" << std::endl;
    auto* runtime = Runtime::instance();
    auto* stack = new StackPool(128 * 1024, 2);
    runtime->spawn(
        [runtime]() {
            std::cout << "THREAD 1 STARTING" << std::endl;
            int id = 1;
            for (int i = 0; i < 10; i++) {
                std::cout << "thread: " << id << " counter: " << i << std::endl;
                runtime->yield();
            }
            std::cout << "THREAD 1 FINISHED" << std::endl;
        },
        stack);
    runtime->spawn(
        [runtime]() {
            std::cout << "THREAD 2 STARTING" << std::endl;
            int id = 2;
            for (int i = 0; i < 15; i++) {
                std::cout << "thread: " << id << " counter: " << i << std::endl;
                runtime->yield();
            }
            std::cout << "THREAD 2 FINISHED" << std::endl;
        },
        stack);
    runtime->spawn(
        [runtime]() {
            std::cout << "THREAD 3 STARTING" << std::endl;
            int id = 3;
            for (int i = 0; i < 20; i++) {
                std::cout << "thread: " << id << " counter: " << i << std::endl;
                runtime->yield();
            }
            std::cout << "THREAD 3 FINISHED" << std::endl;
        },
        stack);
    runtime->run();
    delete stack;
    std::cout << "All Done" << std::endl;
}
