//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <functional>
#include <set>

#include "tento/base/Common.hpp"
#include "tento/base/Duration.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Timestamp.hpp"
#include "tento/net/Alias.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/EventLoop.hpp"
#include "tento/net/Timer.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class TimerQueue : NonCopyable {
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    /// Create a timer to schedule after `interval` from `when`,
    /// returns an unique id identify this timer.
    /// Must be thread safe. Usually be called from other threads.
    TimerId AddTimer(Timestamp when, Duration interval, TimerCallback cb);

    /// Cancel a timer by unique id.
    void CancelTimer(TimerId timerId);

private:
    using TimeEntry = std::pair<Timestamp, Timer*>;
    using IdEntry = std::pair<uint64_t, Timer*>;    /// same as TimeId.
    using TimerSetSortByTime = std::set<TimeEntry>;
    using TimerSetSortById = std::set<IdEntry>;

private:
    void handleRead();
    std::vector<TimeEntry> getExpiredTimers(Timestamp now);
    void reset(const std::vector<TimeEntry>& entries, Timestamp now);
    bool insert(Timer* timer);

private:
    EventLoop* ownerLoop_;

    int timerFd_;
    /// Life time of the channel is managed by TimerQueue.
    std::unique_ptr<Channel> timerFdChannel_;

    /// timers_ and activeTimers store the same timers.
    TimerSetSortByTime timers_;
    TimerSetSortById activeTimers_;
    TimerSetSortById cancelingTimers_;
    bool callingExpiredTimers_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
