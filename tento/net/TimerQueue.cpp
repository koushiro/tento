//
// Created by koushiro on 8/23/18.
//

#include "tento/net/TimerQueue.hpp"

#include <sys/timerfd.h>
#include <unistd.h>

#include "tento/base/Logger.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

int TimerFdCreate() {
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    LOG_TRACE("TimerFdCreate, fd = {}", timerfd);
    if (timerfd == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("TimerFdCreate - timerfd_create() failed, "
                     "an error '{}' occurred", strerror(errorCode));
        abort();
    }
    return timerfd;
}

void TimerFdSet(int timerfd, Timestamp when) {
//    LOG_TRACE("TimerFdSet, fd = {}, when = {}", timerfd, when);
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
        auto errorCode = errno;
        LOG_CRITICAL("TimerFdSet - timerfd_settime() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

void TimerFdRead(int timerfd) {
//    LOG_TRACE("TimerFdRead, fd = {}", timerfd);
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
}

TimerPtr TimerQueue::AddTimer(Timestamp when, Duration interval, const TimerCallback& cb) {
    auto timer = std::make_shared<Timer>(when, interval, cb);
    LOG_TRACE("TimerQueue::AddTimer, id = {}, when = {}", timer->Id(), when);
    /// Transfer actual work to I/O thread, thread safe.
    ownerLoop_->RunInLoop(
        [=]() {
            bool earliestChanged = insert(timer);
            if (earliestChanged) {
                TimerFdSet(timerFd_, when);
            }
        }
    );
    return timer;
}

TimerPtr TimerQueue::AddTimer(Timestamp when, Duration interval, TimerCallback&& cb) {
    auto timer = std::make_shared<Timer>(when, interval, std::move(cb));
    LOG_TRACE("TimerQueue::AddTimer, id = {}, when = {}", timer->Id(), when);
    /// Transfer actual work to I/O thread, thread safe.
    ownerLoop_->RunInLoop(
        [=]() {
            bool earliestChanged = insert(timer);
            if (earliestChanged) {
                TimerFdSet(timerFd_, when);
            }
        }
    );

    return timer;
}

void TimerQueue::CancelTimer(TimerPtr timer) {
    LOG_TRACE("TimerQueue::CancelTimer, id = {}", timer->Id());
    /// Transfer actual work to I/O thread, thread safe.
    ownerLoop_->RunInLoop(
        [=]() {
            auto it = activeTimerIds_.find(timer->Id());
            if (it != activeTimerIds_.end()) {
                assert(*it == timer->Id());
                timers_.erase(timer);
                activeTimerIds_.erase(it);
            } else if (callingExpiredTimers_) {
                cancelingTimerIds_.insert(timer->Id());
            }
            assert(timers_.size() == activeTimerIds_.size());
        }
    );
}

void TimerQueue::handleRead() {
    assert(ownerLoop_->IsInLoopThread());
    TimerFdRead(timerFd_);

    Timestamp now = Timestamp::Now();
    std::vector<TimerPtr> timers = getExpiredTimers(now);

    callingExpiredTimers_ = true;
    cancelingTimerIds_.clear();
    for (auto& timer : timers) {
        timer->Start();
    }
    callingExpiredTimers_ = false;

    reset(timers, now);
}

std::vector<TimerPtr> TimerQueue::getExpiredTimers(Timestamp now) {
    std::vector<TimerPtr> timers;
    auto key = std::make_shared<Timer>(now, Duration{0, 0}, nullptr);
    auto end = timers_.lower_bound(key);
    assert(end == timers_.end() || now < end->get()->Expiration());

    timers.assign(timers_.begin(), end);
    timers_.erase(timers_.begin(), end);
    for (auto& timer : timers) {
        activeTimerIds_.erase(timer->Id());
    }

    return timers;
}

void TimerQueue::reset(const std::vector<TimerPtr>& timers, Timestamp now) {
    for (const auto& timer : timers) {
        /// If the timer is periodic and it is not canceled, then restart the timer.
        if (timer->IsPeriodic() &&
            cancelingTimerIds_.find(timer->Id()) == cancelingTimerIds_.end()) {
            timer->Restart(now);
            insert(timer);
        }
    }

    /// Get earliest expiration time from timers_,
    /// set the time by calling timerfd_settime.
    if (!timers_.empty()) {
        auto timer = timers_.begin()->get();
        Timestamp nextExpire = timer->Expiration();
        assert(nextExpire.IsValid());
        TimerFdSet(timerFd_, nextExpire);
    }
}

bool TimerQueue::insert(TimerPtr timer) {
    assert(timers_.size() == activeTimerIds_.size());
    bool earliestChanged = false;
    auto earliestTimer = timers_.begin()->get();
    if (timers_.empty() || timer->Expiration() < earliestTimer->Expiration()) {
        earliestChanged = true;
    }
    timers_.insert(timer);
    activeTimerIds_.insert(timer->Id());
    assert(timers_.size() == activeTimerIds_.size());
    return earliestChanged;
}

NAMESPACE_END(net)
NAMESPACE_END(tento)