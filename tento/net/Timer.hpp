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
#include "tento/net/Alias.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Timer : NonCopyable {
public:
    Timer(Timestamp when, Duration interval, TimerCallback cb);
    ~Timer();

    void Start() const;
    void Restart(Timestamp now);

    uint64_t Id()                 const { return id_; }
    Timestamp Expiration()        const { return expiration_; }
    bool IsExpired(Timestamp now) const { return now >= expiration_; }
    bool IsPeriodic()             const { return periodic_; }

private:
    static std::atomic_uint64_t counter_;

    uint64_t id_;           /// Timer unique ID.
    Timestamp expiration_;  /// The expiration timestamp when the timer is invoked.
    Duration interval_;     /// The interval of periodic timer.
    bool periodic_;         /// The flag that whether the timer is periodic.
    TimerCallback callback_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
