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

#include <gtest/gtest.h>

#include <iostream>
#include <string>

#include "Promise.h"

namespace co {
TEST(Promise, thenAfterFulfilled) {
    int times = 0;
    co::Promise<int>::resolve(1)
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 1);
            return 2;
        })
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 2);
            return std::string("hello world");
        })
        .then([&times](const std::string& data) {
            times++;
            EXPECT_EQ(data, "hello world");
            return "hello world";
        })
        .then([&times](const char* data) {
            times++;
            EXPECT_STREQ(data, "hello world");
            return nullptr;
        })
        .then([&times](std::nullptr_t data) {
            times++;
            EXPECT_EQ(data, nullptr);
            return "anything";
        });
    EXPECT_EQ(times, 5);
}

TEST(Promise, thenBeforeFulfilled) {
    co::Promise<int>::Resolver* resolver = nullptr;
    auto promise =
        co::Promise<int>([&resolver](co::Promise<int>::Resolver* _resolver) {
            resolver = _resolver;
        });
    int times = 0;
    promise
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 1);
            return 2;
        })
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 2);
            return std::string("hello world");
        })
        .then([&times](std::string data) {
            times++;
            EXPECT_EQ(data, "hello world");
            return "hello world";
        })
        .then([&times](const char* data) {
            times++;
            EXPECT_STREQ(data, "hello world");
            return nullptr;
        })
        .then([&times](std::nullptr_t data) {
            times++;
            EXPECT_EQ(data, nullptr);
            return "anything";
        });
    EXPECT_EQ(times, 0);
    resolver->resolve(1);
    EXPECT_EQ(times, 5);
}

TEST(Promise, thenWithReturnFulfilledPromise) {
    int times = 0;
    co::Promise<int>::resolve(1)
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 1);
            return co::Promise<int>::resolve(2);
        })
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 2);
            return "anything";
        });
    EXPECT_EQ(times, 2);
}

TEST(Promise, thenWithReturnRejectedPromise) {
    int times = 0;
    co::Promise<int>::resolve(1)
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 1);
            return co::Promise<int>::reject(
                std::runtime_error("error message"));
        })
        .then([](int data) {
            EXPECT_TRUE(false);
            return "anything";
        })
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return nullptr;
        });
    EXPECT_EQ(times, 2);
}

TEST(Promise, thenWithReturnPendingPromiseAndResolve) {
    co::Promise<int>::Resolver* resolver = nullptr;
    int times = 0;
    co::Promise<int>::resolve(1)
        .then([&times, &resolver](int data) {
            times++;
            EXPECT_EQ(data, 1);
            return co::Promise<int>(
                [&resolver](co::Promise<int>::Resolver* _resolver) {
                    resolver = _resolver;
                });
        })
        .then([&times](int data) {
            times++;
            EXPECT_EQ(data, 2);
            return "anything";
        });
    EXPECT_EQ(times, 1);
    resolver->resolve(2);
    EXPECT_EQ(times, 2);
}

TEST(Promise, thenWithReturnPendingPromiseAndReject) {
    co::Promise<int>::Resolver* resolver = nullptr;
    int times = 0;
    co::Promise<int>::resolve(1)
        .then([&times, &resolver](int data) {
            times++;
            EXPECT_EQ(data, 1);
            return co::Promise<int>(
                [&resolver](co::Promise<int>::Resolver* _resolver) {
                    resolver = _resolver;
                });
        })
        .then([](int data) {
            EXPECT_TRUE(false);
            return "anything";
        })
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return nullptr;
        });
    EXPECT_EQ(times, 1);
    resolver->reject(std::runtime_error("error message 2"));
    EXPECT_EQ(times, 2);
}

TEST(Promise, errorAfterRejected) {
    int times = 0;
    co::Promise<int>::reject(std::runtime_error("error message"))
        .then([](int data) {
            EXPECT_TRUE(false);
            return "anything";
        })
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return nullptr;
        })
        .then([&times](std::nullptr_t) {
            times++;
            return "hello world";
        })
        .then([&times](const char* data) {
            times++;
            EXPECT_STREQ(data, "hello world");
            return nullptr;
        })
        .error([](const std::exception_ptr& e) {
            EXPECT_TRUE(false);
            return nullptr;
        });
    EXPECT_EQ(times, 3);
}

TEST(Promise, errorBeforeRejected) {
    co::Promise<int>::Resolver* resolver = nullptr;
    auto promise =
        co::Promise<int>([&resolver](co::Promise<int>::Resolver* _resolver) {
            resolver = _resolver;
        });
    int times = 0;
    promise
        .then([](int data) {
            EXPECT_TRUE(false);
            return "anything";
        })
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return nullptr;
        })
        .then([&times](std::nullptr_t) {
            times++;
            return "hello world";
        })
        .then([&times](const char* data) {
            times++;
            EXPECT_STREQ(data, "hello world");
            return nullptr;
        })
        .error([](const std::exception_ptr& e) {
            EXPECT_TRUE(false);
            return nullptr;
        });
    EXPECT_EQ(times, 0);
    resolver->reject(std::runtime_error("error message"));
    EXPECT_EQ(times, 3);
}

TEST(Promise, errorWithReturnFulfilledPromise) {
    int times = 0;
    co::Promise<int>::reject(std::runtime_error("error message"))
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return co::Promise<std::nullptr_t>::resolve(nullptr);
        })
        .then([&times](std::nullptr_t) {
            times++;
            return "anything";
        })
        .error([](const std::exception_ptr& e) {
            EXPECT_TRUE(false);
            return nullptr;
        });
    EXPECT_EQ(times, 2);
}

TEST(Promise, errorWithReturnRejectedPromise) {
    int times = 0;
    co::Promise<int>::reject(std::runtime_error("error message"))
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return co::Promise<std::nullptr_t>::reject(
                std::runtime_error("error message 2"));
        })
        .then([](std::nullptr_t) {
            EXPECT_TRUE(false);
            return "anything";
        })
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return nullptr;
        });
    EXPECT_EQ(times, 2);
}

TEST(Promise, errorWithReturnPendingPromiseAndResolve) {
    co::Promise<std::nullptr_t>::Resolver* resolver = nullptr;
    int times = 0;
    co::Promise<int>::reject(std::runtime_error("error message"))
        .error([&times, &resolver](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return co::Promise<std::nullptr_t>(
                [&resolver](co::Promise<std::nullptr_t>::Resolver* _resolver) {
                    resolver = _resolver;
                });
        })
        .then([&times](std::nullptr_t) {
            times++;
            return "anything";
        });
    EXPECT_EQ(times, 1);
    resolver->resolve(nullptr);
    EXPECT_EQ(times, 2);
}

TEST(Promise, errorWithReturnPendingPromiseAndReject) {
    co::Promise<std::nullptr_t>::Resolver* resolver = nullptr;
    int times = 0;
    co::Promise<int>::reject(std::runtime_error("error message"))
        .error([&times, &resolver](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return co::Promise<std::nullptr_t>(
                [&resolver](co::Promise<std::nullptr_t>::Resolver* _resolver) {
                    resolver = _resolver;
                });
        })
        .then([](std::nullptr_t) {
            EXPECT_TRUE(false);
            return "anything";
        })
        .error([&times](const std::exception_ptr& e) {
            times++;
            EXPECT_THROW(std::rethrow_exception(e), std::runtime_error);
            return nullptr;
        });
    EXPECT_EQ(times, 1);
    resolver->reject(std::runtime_error("error message 2"));
    EXPECT_EQ(times, 2);
}

TEST(Promise, triggrtAtOtherCoroutine) {
    auto* runtime = Runtime::instance();
    Promise<int>::Resolver* resolver = nullptr;
    runtime->spawn([&resolver]() {
        bool resolved = false;
        auto current = Routine::current();
        Promise<int>([&resolver](Promise<int>::Resolver* _resolver) {
            resolver = _resolver;
        }).then([current, &resolved](int data) {
            EXPECT_EQ(data, 42);
            EXPECT_EQ(Routine::current(), current);
            resolved = true;
            return nullptr;
        });
        while (!resolved) {
            Runtime::instance()->yield();
        }
    });
    runtime->spawn([&resolver]() { resolver->resolve(42); });
    runtime->run();
}

TEST(Promise, awaitThenResolve) {
    auto* runtime = Runtime::instance();
    Promise<int>::Resolver* resolver = nullptr;
    runtime->spawn([&resolver]() {
        Promise<int> promise([&resolver](Promise<int>::Resolver* _resolver) {
            resolver = _resolver;
        });
        auto data = await promise;
        EXPECT_EQ(data, 42);
    });
    runtime->spawn([&resolver]() { resolver->resolve(42); });
    runtime->run();
}

TEST(Promise, awaitThenReject) {
    auto* runtime = Runtime::instance();
    Promise<int>::Resolver* resolver = nullptr;
    runtime->spawn([&resolver]() {
        Promise<int> promise([&resolver](Promise<int>::Resolver* _resolver) {
            resolver = _resolver;
        });
        EXPECT_THROW(await promise, std::runtime_error);
    });
    runtime->spawn([&resolver]() {
        resolver->reject(std::runtime_error("error message"));
    });
    runtime->run();
}

TEST(Promise, awaitWithFulfilledPromise) {
    auto* runtime = Runtime::instance();
    auto promise = Promise<int>::resolve(42);
    auto data = await promise;
    EXPECT_EQ(data, 42);
}

TEST(Promise, awaitWithRejectedPromise) {
    auto* runtime = Runtime::instance();
    auto promise = Promise<int>::reject(std::runtime_error("error message"));
    EXPECT_THROW(await promise, std::runtime_error);
}

TEST(Promise, awaitValue) {
    int number = 42;
    EXPECT_EQ(await 42, 42);
    EXPECT_EQ(await number, 42);
}

}  // namespace co
