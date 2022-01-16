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
#ifndef PROMISE_INTERNAL_H
#define PROMISE_INTERNAL_H

#include <cassert>
#include <exception>
#include <forward_list>
#include <functional>
#include <type_traits>

#include "Routine.h"
#include "Runtime.h"

namespace co {

template <typename T>
class Promise;

namespace internal {
class BasePromise {
   public:
    enum class State { Pending, Fulfilled, Rejected };

   public:
    BasePromise() = default;
    virtual ~BasePromise() = default;

   protected:
    void _reject(const std::exception_ptr& e) {
        this->exception_ = e;
        this->settle(State::Rejected);
    }
    virtual void settle(State state) = 0;

   protected:
    State state_ = State::Pending;
    std::exception_ptr exception_;
};

class BaseAction {
   public:
    BaseAction() : routine_(Routine::current()) {}
    virtual ~BaseAction() = default;

   public:
    void reject(const std::exception& e) {
        this->reject(std::make_exception_ptr(e));
    };
    virtual void reject(const std::exception_ptr& e) = 0;

   protected:
    Routine* routine_ = nullptr;
    bool settled_ = false;
};

template <typename T>
class ActionFrom : virtual public BaseAction {
   public:
    ActionFrom(Promise<T>* source) {
        if (!source) return;
        assert(source->wait_for_);
        source->wait_for_->sub_actions_.push_front(this);
    };
    virtual ~ActionFrom() = default;

   public:
    virtual void resolve(const T& value) = 0;
    void resolve(const Promise<T>& promise) {
        switch (promise.state_) {
            case Promise<T>::State::Pending: {
                assert(promise.wait_for_);
                promise.wait_for_->sub_actions_.push_front(this);
                break;
            }
            case Promise<T>::State::Fulfilled: {
                this->resolve(promise.value());
                break;
            }
            case Promise<T>::State::Rejected: {
                this->reject(promise.exception_);
                break;
            }
        }
    }

   protected:
    friend class Promise<T>;
};

template <typename R>
class ActionTo : virtual public BaseAction {
   public:
    ActionTo(Promise<R>* target) : target_(target) {
        target_->wait_for_ = this;
    };
    virtual ~ActionTo() = default;

   public:
    void reject(const std::exception_ptr& e) override {
        if (this->settled_) return;
        this->settled_ = true;
        if (this->routine_ != Routine::current()) {
            this->routine_->defer([this, e]() {
                this->settled_ = false;
                this->reject(e);
            });
            return;
        }
        if (this->target_) this->target_->_reject(e);
        for (auto* action : this->sub_actions_) {
            action->reject(e);
        }
        this->sub_actions_.clear();
        delete this;
    }

   protected:
    friend class Promise<R>;
    friend class ActionFrom<R>;
    Promise<R>* target_;
    std::forward_list<ActionFrom<R>*> sub_actions_ = {};
};

template <typename T, typename R, typename FR = R>
class Action : public ActionFrom<T>, public ActionTo<R> {
   public:
    Action() = delete;
    Action(const Action<T, R, FR>&) = delete;
    Action(Promise<T>* source, Promise<R>* target, std::function<FR(T)> then)
        : ActionFrom<T>(source), ActionTo<R>(target), then_(then){};
    virtual ~Action() = default;

   public:
    void resolve(const T& value) override {
        if (this->settled_) return;
        this->settled_ = true;
        if (this->routine_ != Routine::current()) {
            this->routine_->defer([this, value]() {
                this->settled_ = false;
                this->resolve(value);
            });
            return;
        }
        FR r = this->then_(value);
        if (this->target_) this->target_->_resolve(r);
        for (auto* action : this->sub_actions_) {
            action->resolve(r);
        }
        this->sub_actions_.clear();
        delete this;
    }
    void reject(const std::exception_ptr& e) override {
        ActionTo<R>::reject(e);
    }

   protected:
    std::function<FR(T)> then_ = nullptr;
};

template <typename T, typename FR = std::nullptr_t>
class CatchAction final : public Action<T, std::nullptr_t> {
   private:
    using R = std::nullptr_t;

   public:
    CatchAction() = delete;
    CatchAction(const Action<T, FR>&) = delete;
    CatchAction(Promise<T>* source, Promise<R>* target,
                std::function<FR(const std::exception_ptr&)> _catch)
        : Action<T, R>(source, target, [](const T&) { return nullptr; }),
          catch_(_catch){};
    virtual ~CatchAction() = default;

   public:
    void reject(const std::exception_ptr& e) override {
        if (this->settled_) return;
        this->settled_ = true;
        if (this->routine_ != Routine::current()) {
            this->routine_->defer([this, e]() {
                this->settled_ = false;
                this->reject(e);
            });
            return;
        }
        FR r = this->catch_(e);
        if (this->target_) this->target_->_resolve(r);
        for (auto* action : this->sub_actions_) {
            action->resolve(r);
        }
        this->sub_actions_.clear();
        delete this;
    }

   protected:
    std::function<FR(const std::exception_ptr&)> catch_ = nullptr;
};
}  // namespace internal
}  // namespace co

#endif  // PROMISE_INTERNAL_H
