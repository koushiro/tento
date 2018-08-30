//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <atomic>
#include <functional>
#include <memory>

#include "tento/base/Common.hpp"
#include "tento/base/Duration.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Timestamp.hpp"
#include "tento/net/Callbacks.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Timer : public NonCopyable {
public:
    Timer(Timestamp when, Duration interval, TimerCallback cb)
    : id_(++counter_),
      expiration_(when),
      interval_(interval),
      periodic_(!interval.IsZero()),
      callback_(std::move(cb))
    {}
    ~Timer() = default;

    void Start() const {
        callback_();
    }

    void Cancel() {

    }

    void Restart(Timestamp now) {
        if (periodic_) {
            expiration_ = now + interval_;
        } else {
            expiration_ = Timestamp::Invalid();
        }
    }

    uint64_t Id() const { return id_; }

    Timestamp Expiration() const { return expiration_; }

    bool operator< (const Timer& rhs) const {
        return expiration_ < rhs.expiration_;
    }

private:
    static std::atomic_uint64_t counter_;

    uint64_t id_;           /// Timer unique ID.
    Timestamp expiration_;  /// The expiration timestamp when the timer is invoked.
    Duration interval_;     /// The interval of periodic timer.
    bool periodic_;         /// The flag that whether the timer is periodic.
    TimerCallback callback_;
};

std::atomic_uint64_t Timer::counter_(0);

//using TimerPtr = std::unique_ptr<Timer>;

NAMESPACE_END(net)
NAMESPACE_END(tento)
