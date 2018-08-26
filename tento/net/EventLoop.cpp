//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EventLoop.hpp"

#include "tento/base/Logger.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/Poller.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

__thread EventLoop* loopInThisThread = nullptr;

const int EventLoop::kPollTimeMs = 10000;

EventLoop::EventLoop()
    : tid_(std::this_thread::get_id()),
      looping_(false),
      quit_(false),
      eventHandling_(false),
      poller_(Poller::NewDefaultPoller(this)),
      currentActiveChannel_(nullptr)
{
    LOG_TRACE("EventLoop Created {} in thread {}", (void*)this, tid_);
    if (loopInThisThread) {
        LOG_CRITICAL("Another EventLoop {} exists in this thread {}",
                     (void*)loopInThisThread, tid_);
    } else {
        loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    loopInThisThread = nullptr;
}

void EventLoop::Loop() {
    assert(!looping_);
    AssertInLoopThread();
    looping_ = true;
    LOG_TRACE("EventLoop {} start looping", (void*)this);

    while (!quit_) {
        activeChannels_.clear();

        pollReturnTime_ = poller_->Poll(kPollTimeMs, &activeChannels_);

        // TODO sort channel by priority
        eventHandling_ = true;
        for (auto channel : activeChannels_) {
            currentActiveChannel_ = channel;
            currentActiveChannel_->HandleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = nullptr;
        eventHandling_ = false;
    }

    LOG_TRACE("EventLoop {} stop looping", (void*)this);
    looping_ = false;
}

void EventLoop::Quit() {
    quit_ = true;
    if (!IsInLoopThread()) {

    }
}

void EventLoop::UpdateChannel(Channel* channel) {
    assert(channel->OwnerLoop() == this);
    AssertInLoopThread();
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
    assert(channel->OwnerLoop() == this);
    AssertInLoopThread();
    if (eventHandling_) {
        assert(currentActiveChannel_ == channel ||
            std::find(activeChannels_.begin(), activeChannels_.end(), channel)
            == activeChannels_.end()
        );
    }
    poller_->RemoveChannel(channel);
}

void EventLoop::abortNotInLoopThread() {
    LOG_CRITICAL("EventLoop::abortNotInLoopThread - "
                 "EventLoop {} was created in tid = {}, current tid = {}",
                 (void*)this, tid_, std::this_thread::get_id());
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
