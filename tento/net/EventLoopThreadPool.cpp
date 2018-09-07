//
// Created by koushiro on 9/5/18.
//

#include "tento/net/EventLoopThreadPool.hpp"

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, uint32_t numThread)
    : baseLoop_(baseLoop),
      numThread_(numThread),
      next_(0)
{
    LOG_TRACE("EventLoopThreadPool::EventLoopThreadPool, "
              "base loop = {}, thread num = {}", (void*)baseLoop_, numThread_);
    status_ = Status::kStarting;

    /// Single thread condition (there is only an event loop - baseLoop_).
    if (numThread_ == 0) {
        status_ = Status::kRunning;
        return;
    }

    for (uint32_t i = 0; i < numThread_; ++i) {
        auto thread = std::make_unique<EventLoopThread>();
        LOG_TRACE("index = {}, thread-{} started", i, thread->ThreadId());
        threads_.push_back(std::move(thread));
    }

    LOG_TRACE("thread pool (num = {}) totally started", numThread_);
    status_ = Status::kRunning;
}

EventLoopThreadPool::~EventLoopThreadPool() {
    LOG_TRACE("EventLoopThreadPool::~EventLoopThreadPool", "");
    Stop();
}

void EventLoopThreadPool::Stop() {
    if (!IsRunning()) {
        status_ = Status::kStopped;
        return;
    }

    LOG_TRACE("EventLoopThreadPool::Stop", "");
    status_ = Status::kStopping;

    /// Single thread condition (there is only an event loop - baseLoop_).
    if (numThread_ == 0) {
        status_ = Status::kStopped;
        return;
    }

    /// when all the working thread have stopped,
    /// status_ will be stored with kStopped.
    uint32_t i = 0;
    for (auto& thread : threads_) {
        LOG_TRACE("index = {}, thread-{} will exit", i, thread->ThreadId());
        thread->Stop();
        ++i;
    }

    LOG_TRACE("thread pool (num = {}) totally exited", threads_.size());
    status_ = Status::kStopped;
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
    LOG_TRACE("EventLoopThreadPool::GetNextLoop", "");
    EventLoop* loop = baseLoop_;

    if (IsRunning() && !threads_.empty()) {
        uint64_t next = next_++;
        next %= threads_.size();
        loop = (threads_[next])->GetLoop();
    }

    return loop;
}

EventLoop* EventLoopThreadPool::GetNextLoopWithHash(uint64_t hash) {
    LOG_TRACE("EventLoopThreadPool::GetNextLoopWithHash", "");
    EventLoop* loop = baseLoop_;

    if (IsRunning() && !threads_.empty()) {
        uint64_t next =  hash % threads_.size();
        loop = (threads_[next])->GetLoop();
    }

    return loop;
}

NAMESPACE_END(net)
NAMESPACE_END(tento)