//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EventLoop.hpp"

#include <sys/eventfd.h>

#include "tento/base/Logger.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/EPoller.hpp"
#include "tento/net/TimerQueue.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

int EventFdCreate() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    LOG_TRACE("EventFdCreate, fd = {}", evtfd);
    if (evtfd == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("EventFdCreate failed, "
                     "an error '{}' occurred", strerror(errorCode));
        abort();
    }
    return evtfd;
}

void EventFdWrite(int eventFd) {
//    LOG_TRACE("EventFdWrite, fd = {}", eventFd);
    uint64_t one = 1;
    ssize_t n = write(eventFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventFdWrite() writes {} bytes instead of 8", n);
    }
}

void EventFdRead(int eventFd) {
//    LOG_TRACE("EventFdRead, fd = {}", eventFd);
    uint64_t one = 1;
    ssize_t n = read(eventFd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventFdRead() reads {} bytes instead of 8", n);
    }
}

__thread EventLoop* tEventLoop = nullptr;

EventLoop::EventLoop()
    : tid_(thread_id()),
      eventHandling_(false),
      poller_(std::make_unique<EPoller>(this)),
      timerQueue_(std::make_unique<TimerQueue>(this)),
      callingPendingCallbacks_(false),
      eventFd_(EventFdCreate()),
      eventFdChannel_(std::make_unique<Channel>(this, eventFd_))
{
    LOG_TRACE("EventLoop::EventLoop, EventLoop {} was created in thread {}",
        (void*)this, tid_);
    if (tEventLoop) {
        LOG_CRITICAL("EventLoop::EventLoop,"
                     "Another EventLoop {} exists in this thread {}",
                     (void*)tEventLoop, tid_);
    } else {
        tEventLoop = this;
    }

    eventFdChannel_->SetReadCallback([this]() { EventFdRead(eventFd_); });
    eventFdChannel_->EnableReadEvent();
}

EventLoop::~EventLoop() {
    LOG_TRACE("EventLoop::~EventLoop, "
              "EventLoop {} of thread {} destructs in thread {}",
              (void*)this, tid_, thread_id());
    eventFdChannel_->DisableAllEvents();
    eventFdChannel_->Remove();
    close(eventFd_);
    assert(tEventLoop == this);
    tEventLoop = nullptr;
}

void EventLoop::Run() {
    assert(IsInLoopThread());
    status_ = Status::kStarting;
    LOG_TRACE("EventLoop::Run, EventLoop {} is starting looping", (void*)this);

    while (!IsStopping()) {
        status_ = Status::kRunning;
        activeChannels_.clear();
        poller_->Poll(kPollTimeMs, &activeChannels_);

        eventHandling_ = true;
        for (auto channel : activeChannels_) {
            channel->HandleEvent();
        }
        eventHandling_ = false;
        /// Do pending callbacks from QueueInLoop().
        /// Could do some computing work in I/O thread.
        doPendingCallbacks();
    }

    status_ = Status::kStopped;
    LOG_TRACE("EventLoop::Run, EventLoop {} stopped looping", (void*)this);
}

void EventLoop::doPendingCallbacks() {
    std::vector<Callback> callbacks;
    /// Reduce critical area, avoid deadlock (callback maybe call QueueInLoop).
    callingPendingCallbacks_ = true;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        callbacks.swap(pendingCallbacks_);
    }
    for (const auto& callback : callbacks) {
        callback();
    }
    callingPendingCallbacks_ = false;
}

void EventLoop::Quit() {
    LOG_TRACE("EventLoop::Quit(), status = {}", StatusToString());
    if (IsStopping() || IsStopped()) return;
    status_ = Status::kStopping;
    LOG_TRACE("EventLoop::Quit(), EventLoop {} is stopping looping", (void*)this);
    if (!IsInLoopThread()) {
        WakeUp();
    }
}

void EventLoop::RunInLoop(Callback cb) {
    if (IsInLoopThread()) {
        cb();
    } else {
        QueueInLoop(std::move(cb));
    }
}

void EventLoop::QueueInLoop(Callback cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingCallbacks_.push_back(std::move(cb));
    }
    /// The thread that had called QueueInLoop is not current I/O thread,
    /// or is current I/O thread but current I/O thread is calling pending callbacks.
    /// In above condition, we need to wakeup I/O thread.
    if (!IsInLoopThread() || callingPendingCallbacks_) {
        WakeUp();
    }
}

TimerPtr EventLoop::RunAt(Timestamp time, TimerCallback cb) {
    LOG_TRACE("EventLoop::RunAt, time = {}", time);
    return timerQueue_->AddTimer(time, Duration(0, 0), std::move(cb));
}

TimerPtr EventLoop::RunAfter(Duration delay, TimerCallback cb) {
    auto time = Timestamp::Now() + delay;
    LOG_TRACE("EventLoop::RunAfter, time = {}", time);
    return timerQueue_->AddTimer(time, Duration(0, 0), std::move(cb));
}

TimerPtr EventLoop::RunEvery(Duration interval, TimerCallback cb) {
    auto time = Timestamp::Now() + interval;
    LOG_TRACE("EventLoop::RunEvery, time = {}", time);
    return timerQueue_->AddTimer(time, interval, std::move(cb));
}

void EventLoop::CancelTimer(TimerPtr timer) {
    LOG_TRACE("EventLoop::CancelTimer, timer id = {}", timer->Id());
    return timerQueue_->CancelTimer(timer);
}

void EventLoop::UpdateChannel(Channel* channel) {
    assert(channel->OwnerLoop() == this);
    assert(IsInLoopThread());
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
    assert(channel->OwnerLoop() == this);
    assert(IsInLoopThread());
    if (eventHandling_) {
        assert(std::find(activeChannels_.begin(), activeChannels_.end(), channel)
               == activeChannels_.end());
    }
    poller_->RemoveChannel(channel);
}

void EventLoop::WakeUp() {
    EventFdWrite(eventFd_);
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
