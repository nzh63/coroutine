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

#pragma once
#ifndef PROMISE_H
#define PROMISE_H

#include <cassert>
#include <exception>
#include <forward_list>
#include <functional>
#include <type_traits>
#include <utility>

#include "Runtime.h"
#include "internal/Promise-internal.h"

namespace co {

template <typename T>
class Promise final : public internal::BasePromise {
   public:
    class Resolver;
    using Executor = std::function<void(Resolver*)>;

   public:
    static Promise<T> resolve(const T& value);
    static Promise<T> resolve(T&& value);
    template <typename E>
    static Promise<T> reject(E&& value);

   protected:
    Promise() = default;

   public:
    explicit Promise(Executor executor);
    Promise(const Promise<T>& other);
    Promise(Promise<T>&& other);
    virtual ~Promise();

   public:
    template <typename F, typename R = internal::Awaited<
                              decltype(std::declval<F>()(std::declval<T>()))>>
    Promise<R> then(const F& callback);

    template <typename F>
    Promise<std::nullptr_t> error(const F& callback);

    template <typename F, typename R = internal::Awaited<
                              decltype(std::declval<F>()(nullptr))>>
    Promise<R> finally(const F& callback);

    T await();

   protected:
    void _resolve(const T& val);
    void _resolve(T&& val);
    void _resolve(const Promise<T>& promise);
    void settle(State state) override;

    const T& value() const;

   protected:
    template <typename>
    friend class Promise;
    friend class internal::ActionFrom<T>;
    friend class internal::ActionTo<T>;
    template <typename, typename, typename>
    friend class internal::Action;
    template <typename, typename>
    friend class internal::CatchAction;
    uint8_t buffer_[sizeof(T)];
    internal::ActionTo<T>* wait_for_ = nullptr;
};

template <typename T>
class Promise<T>::Resolver final : private internal::Action<T, T> {
   protected:
    Resolver(Promise<T>* promise);

   public:
    void resolve(const T& value) override;
    void resolve(const Promise<T>& promise);
    template <typename E>
    void reject(E&& e);

    friend Promise<T>;
};

template <typename T>
Promise<T>::Resolver::Resolver(Promise<T>* promise)
    : internal::Action<T, T>(nullptr, promise,
                             [](const T& val) { return val; }) {}

template <typename T>
void Promise<T>::Resolver::resolve(const T& value) {
    internal::Action<T, T>::resolve(value);
}

template <typename T>
void Promise<T>::Resolver::resolve(const Promise<T>& promise) {
    internal::Action<T, T>::resolve(promise);
}

template <typename T>
template <typename E>
void Promise<T>::Resolver::reject(E&& e) {
    internal::Action<T, T>::reject(std::make_exception_ptr(std::forward<E>(e)));
}

template <typename T>
Promise<T> Promise<T>::resolve(const T& value) {
    auto promise = Promise<T>(
        [&value](Promise<T>::Resolver* resolver) { resolver->resolve(value); });
    return promise;
}

template <typename T>
Promise<T> Promise<T>::resolve(T&& value) {
    auto promise = Promise<T>([&value](Promise<T>::Resolver* resolver) {
        resolver->resolve(std::move(value));
    });
    return promise;
}

template <typename T>
template <typename E>
Promise<T> Promise<T>::reject(E&& e) {
    auto promise = Promise<T>([&e](Promise<T>::Resolver* resolver) {
        resolver->reject(std::forward<E>(e));
    });
    return promise;
}

template <typename T>
Promise<T>::Promise(Executor executor) {
    auto* resolver = new Resolver(this);
    executor(resolver);
}

template <typename T>
Promise<T>::Promise(const Promise<T>& other) {
    this->state_ = other.state_;
    switch (this->state_) {
        case State::Pending: {
            new internal::Action<T, T>(const_cast<Promise<T>*>(&other), this,
                                       [](const T& val) { return val; });
            break;
        }
        case State::Fulfilled: {
            new (this->buffer_) T(other.value());
            break;
        }
        case State::Rejected: {
            this->exception_ = other.exception_;
            break;
        }
    }
}

template <typename T>
Promise<T>::Promise(Promise<T>&& other) {
    this->state_ = other.state_;
    switch (this->state_) {
        case State::Pending: {
            this->wait_for_ = other.wait_for_;
            this->wait_for_->target_ = this;
            break;
        }
        case State::Fulfilled: {
            new (this->buffer_) T(std::move(other.value()));
            break;
        }
        case State::Rejected: {
            this->exception_ = std::move(other.exception_);
            break;
        }
    }
}

template <typename T>
Promise<T>::~Promise() {
    if (this->state_ == State::Fulfilled) {
        reinterpret_cast<T*>(this->buffer_)->~T();
    }
    if (this->wait_for_) {
        this->wait_for_->target_ = nullptr;
        this->wait_for_ = nullptr;
    }
}

template <typename T>
template <typename F, typename R>
Promise<R> Promise<T>::then(const F& callback) {
    Promise<R> ret;
    switch (this->state_) {
        case State::Pending: {
            using Action =
                internal::Action<T, R, decltype(callback(this->value()))>;
            new Action(this, &ret, callback);
            break;
        }
        case State::Fulfilled: {
            auto&& val = callback(this->value());
            ret._resolve(std::move(val));
            break;
        }
        case State::Rejected: {
            ret._reject(this->exception_);
            break;
        }
    }
    return ret;
}

template <typename T>
template <typename F, typename R>
Promise<R> Promise<T>::finally(const F& callback) {
    return (*this)
        .then([callback](const T&) { return callback(nullptr); })
        .error([callback](const std::exception_ptr&) {
            return callback(nullptr);
        });
}

template <typename T>
T Promise<T>::await() {
    bool settled = false;
    this->finally([&settled](std::nullptr_t) {
        settled = true;
        return nullptr;
    });
    while (!settled) {
        Runtime::instance()->yield(true);
    }
    if (this->state_ == State::Rejected)
        std::rethrow_exception(this->exception_);
    else
        return this->value();
}

template <typename T>
template <typename F>
Promise<std::nullptr_t> Promise<T>::error(const F& callback) {
    using R = decltype(callback(std::declval<std::exception_ptr>()));
    Promise<std::nullptr_t> ret;
    switch (this->state_) {
        case State::Pending: {
            new internal::CatchAction<T, R>(this, &ret, callback);
            break;
        }
        case State::Fulfilled: {
            return Promise<std::nullptr_t>::resolve(nullptr);
            break;
        }
        case State::Rejected: {
            R r = callback(this->exception_);
            ret._resolve(r);
            break;
        }
    }
    return ret;
}

template <typename T>
void Promise<T>::_resolve(const T& val) {
    if (this->state_ != State::Pending) return;
    new (this->buffer_) T(val);
    this->settle(State::Fulfilled);
}

template <typename T>
void Promise<T>::_resolve(T&& val) {
    if (this->state_ != State::Pending) return;
    new (this->buffer_) T(std::move(val));
    this->settle(State::Fulfilled);
}

template <typename T>
void Promise<T>::_resolve(const Promise<T>& promise) {
    if (this->state_ != State::Pending) return;
    switch (promise.state_) {
        case State::Pending: {
            if (this->wait_for_) {
                assert(this == this->wait_for_->target_);
                this->wait_for_->target_ = nullptr;
                this->wait_for_ = nullptr;
            }
            new internal::Action<T, T>(const_cast<Promise<T>*>(&promise), this,
                                       [](T val) { return val; });
            break;
        }
        case State::Fulfilled: {
            this->_resolve(promise.value());
            break;
        }
        case State::Rejected: {
            this->_reject(promise.exception_);
            break;
        }
    }
}

template <typename T>
void Promise<T>::settle(State state) {
    assert(state != State::Pending);
    this->state_ = state;
    if (this->wait_for_) {
        this->wait_for_->target_ = nullptr;
        this->wait_for_ = nullptr;
    }
}

template <typename T>
const T& Promise<T>::value() const {
    return *reinterpret_cast<const T*>(this->buffer_);
}

#ifndef CO_NO_AWAIT_MACRO
#define await (::co::internal::Awaiter()) %
#endif

}  // namespace co

#endif  // PROMISE_H
