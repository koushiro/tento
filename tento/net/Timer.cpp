//
// Created by koushiro on 9/1/18.
//

#include "tento/net/Timer.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

std::atomic_uint64_t Timer::counter_(0);

Timer::Timer(Timestamp when, Duration interval, TimerCallback cb)
    : id_(++counter_),
      expiration_(when),
      interval_(interval),
      periodic_(!interval.IsZero()),
      callback_(std::move(cb))
{
}

Timer::~Timer() = default;

void Timer::Start() const {
    assert(callback_);
    callback_();
}

void Timer::Restart(Timestamp now) {
    if (periodic_) {
        expiration_ = now + interval_;
    } else {
        expiration_ = Timestamp::Invalid();
    }
}

NAMESPACE_END(net)
NAMESPACE_END(tento)