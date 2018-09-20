//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <set>

#include "tento/base/Common.hpp"
#include "tento/base/Duration.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Timestamp.hpp"
#include "tento/net/Alias.hpp"
#include "tento/net/Timer.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;
class EventLoop;

class TimerQueue : NonCopyable {
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    /// Create a timer to schedule after `interval` from `when`,
    /// returns an pointer that point this timer.
    /// Must be thread safe. Usually be called from other threads.
    TimerPtr AddTimer(Timestamp when, Duration interval, const TimerCallback& cb);
    TimerPtr AddTimer(Timestamp when, Duration interval, TimerCallback&& cb);
    /// Cancel a timer.
    void CancelTimer(TimerPtr timerPtr);

private:
    using TimerSet = std::multiset<TimerPtr, TimerCompare>;
    using TimerIdSet = std::set<uint64_t>;

private:
    void handleRead();
    std::vector<TimerPtr> getExpiredTimers(Timestamp now);
    void reset(const std::vector<TimerPtr>& timers, Timestamp now);
    bool insert(TimerPtr timer);

private:
    EventLoop* ownerLoop_;

    int timerFd_;
    /// Life time of the channel is managed by TimerQueue.
    std::unique_ptr<Channel> timerFdChannel_;

    TimerSet timers_;
    TimerIdSet activeTimerIds_;
    TimerIdSet cancelingTimerIds_;
    bool callingExpiredTimers_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
