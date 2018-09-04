//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EventLoopThread.hpp"

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      started_(false),
      latch_(1)
{
}

EventLoopThread::~EventLoopThread() {
    LOG_TRACE("EventLoopThread destructor, loop = {}", (void*)loop_);
    if (loop_ != nullptr) {
        /// Not 100% race-free,
        /// still a tiny chance to call destructed object, if threadFunc exits just now.
        /// but when EventLoopThread destructs, usually programming is exiting anyway.
        loop_->Quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::StartLoop() {
    assert(!started_);
    started_ = true;

    thread_ = std::thread([this]() {
        EventLoop loop;
        loop_ = &loop;
        latch_.CountDown();
        loop.Run();
        loop_ = nullptr;
    });
    latch_.Wait();

    assert(loop_ != nullptr);
    return loop_;
}

NAMESPACE_END(net)
NAMESPACE_END(teno)
