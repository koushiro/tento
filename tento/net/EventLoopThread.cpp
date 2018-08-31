//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EventLoopThread.hpp"
#include "EventLoopThread.hpp"


NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      started_(false),
      latch_(1)
{
}

EventLoopThread::~EventLoopThread() {
    assert(started_);
    if (loop_ != nullptr) {
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
