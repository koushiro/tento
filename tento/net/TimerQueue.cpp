//
// Created by koushiro on 8/23/18.
//

#include "tento/net/TimerQueue.hpp"

#include <sys/timerfd.h>
#include <unistd.h>

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

int TimerFdCreate() {
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd == -1) {
        LOG_ERROR("timerfd_create failed, "
                  "an error '{}' occurred", strerror(errno));
    }
    return timerfd;
}

void TimerFdSet(int timerfd, Timestamp when) {
    // wake up event loop by timerfd_settime()
    struct itimerspec newValue, oldValue;
    memset(&newValue, 0, sizeof(newValue));
    memset(&oldValue, 0, sizeof(oldValue));
    Duration duration = when.DurationSince(Timestamp::Now());
    struct timespec ts;
    ts.tv_sec = duration.SecsPart();
    ts.tv_nsec = duration.SubSecNanos();
    newValue.it_value = ts;
    int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret == -1) {
        LOG_ERROR("timerfd_settime failed, "
                  "an error '{}' occurred", strerror(errno));
    }
}

void TimerFdRead(int timerfd) {
    uint64_t howmany;
    ssize_t n = read(timerfd, &howmany, sizeof(howmany));
    if (n != sizeof(howmany)) {
        LOG_ERROR("TimerFdRead() reads {} bytes instead of 8", n);
    }
}

TimerQueue::TimerQueue(EventLoop* loop)
    : ownerLoop_(loop),
      timerFd_(TimerFdCreate()),
      timerFdChannel_(std::make_unique<Channel>(loop, timerFd_)),
      timers_(),
      activeTimers_(),
      cancelingTimers_(),
      callingExpiredTimers_(false)
{
    timerFdChannel_->SetReadCallback([this]() { handleRead(); });
    /// we are always reading the timerfd, we disarm it with timerfd_settime.
    timerFdChannel_->EnableReadEvent();
}

TimerQueue::~TimerQueue() {
    timerFdChannel_->DisableAllEvents();
    timerFdChannel_->Remove();
    close(timerFd_);
    for (auto& entry : timers_) {
        auto timer = entry.second;
        delete timer;
    }
}

TimerId TimerQueue::AddTimer(Timestamp when, Duration interval, TimerCallback cb) {
    auto timer = new Timer(when, interval, std::move(cb));
    /// Transfer actual work to I/O thread, thread safe.
    ownerLoop_->RunInLoop([&]() {
        bool earliestChanged = insert(timer);
        if (earliestChanged) {
            TimerFdSet(timerFd_, when);
        }
    });
    return TimerId(timer->Id(), timer);
}

void TimerQueue::CancelTimer(TimerId timerId) {
    /// Transfer actual work to I/O thread, thread safe.
    ownerLoop_->RunInLoop([&]() {
        auto it = activeTimers_.find(timerId);
        if (it != activeTimers_.end()) {
            auto timer = it->second;
            timers_.erase(TimeEntry(timer->Expiration(), timer));
            activeTimers_.erase(it);
            delete timer;
        } else if (callingExpiredTimers_) {
            cancelingTimers_.insert(timerId);
        }
        assert(timers_.size() == activeTimers_.size());
    });
}

void TimerQueue::handleRead() {
    ownerLoop_->AssertInLoopThread();
    TimerFdRead(timerFd_);

    Timestamp now = Timestamp::Now();
    std::vector<TimeEntry> entries = getExpiredTimers(now);

    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for (auto& entry : entries) {
        auto timer = entry.second;
        timer->Start();
    }
    callingExpiredTimers_ = false;

    reset(entries, now);
}

std::vector<TimerQueue::TimeEntry> TimerQueue::getExpiredTimers(Timestamp now) {
    std::vector<TimeEntry> entries;
    TimeEntry timeEntry(now, nullptr);
    auto end = timers_.lower_bound(timeEntry);
    assert(end == timers_.end() || now < end->second->Expiration());

    entries.assign(timers_.begin(), end);
    timers_.erase(timers_.begin(), end);
    for (auto& entry : entries) {
        auto timer = entry.second;
        activeTimers_.erase(IdEntry(timer->Id(), timer));
    }

    return entries;
}

void TimerQueue::reset(const std::vector<TimeEntry>& entries, Timestamp now) {
    for (const auto& entry : entries) {
        auto timer = entry.second;
        IdEntry idEntry(timer->Id(), timer);
        /// If the timer is periodic and it is not canceled, then restart the timer,
        /// or delete the timer.
        if (timer->IsPeriodic() &&
            cancelingTimers_.find(idEntry) == cancelingTimers_.end()) {
            timer->Restart(now);
            insert(timer);
        } else {
            delete timer;
        }
    }

    /// Get earliest expiration time from timers_,
    /// set the time by calling timerfd_settime.
    if (!timers_.empty()) {
        auto timer = timers_.begin()->second;
        Timestamp nextExpire = timer->Expiration();
        assert(nextExpire.IsValid());
        TimerFdSet(timerFd_, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer) {
    assert(timers_.size() == activeTimers_.size());
    bool earliestChanged = false;
    if (timers_.empty() || timer->Expiration() < timers_.begin()->first) {
        earliestChanged = true;
    }
    timers_.insert(TimeEntry(timer->Expiration(), timer));
    activeTimers_.insert(IdEntry(timer->Id(), timer));
    assert(timers_.size() == activeTimers_.size());
    return earliestChanged;
}

NAMESPACE_END(net)
NAMESPACE_END(tento)