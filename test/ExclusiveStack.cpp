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

#include <gtest/gtest.h>

#include <iostream>

#include "coroutine.h"

using namespace CO;

TEST(CO, ExclusiveStack) {
    int count = 0;
    auto* runtime = Runtime::instance();
    runtime->spawn([runtime, &count]() {
        for (int i = 0; i < 11; i++) {
            count++;
            EXPECT_EQ(count, i * 3 + 1);
            runtime->yield();
        }
    });
    runtime->spawn([runtime, &count]() {
        for (int i = 0; i < 10; i++) {
            count++;
            EXPECT_EQ(count, i * 3 + 2);
            runtime->yield();
        }
    });
    runtime->spawn([runtime, &count]() {
        for (int i = 0; i < 10; i++) {
            count++;
            EXPECT_EQ(count, i * 3 + 3);
            runtime->yield();
        }
    });
    runtime->run();
}