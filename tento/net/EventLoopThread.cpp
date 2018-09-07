//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EventLoopThread.hpp"

#include "tento/base/CountDownLatch.hpp"
#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

EventLoopThread::EventLoopThread()
    : loop_(nullptr)
{
    LOG_TRACE("EventLoopThread::EventLoopThread", "");
    status_ = Status::kStarting;

    CountDownLatch startLatch{1};

    thread_ = std::make_unique<Thread>(
        [this, &startLatch]() {
            status_ = Status::kRunning;
            tid_ = thread_id();

            EventLoop loop;
            loop_ = &loop;

            startLatch.CountDown();
            loop_->Run();

            LOG_TRACE("EventLoopThread::EventLoopThread, "
                      "EventLoop = {} stopped.", (void*)loop_);
            status_ = Status::kStopped;
            loop_ = nullptr;
        }
    );

    startLatch.Wait();
    assert(loop_ != nullptr);
}

EventLoopThread::~EventLoopThread() {
    LOG_TRACE("EventLoopThread::~EventLoopThread", "");
    Stop();
}

void EventLoopThread::Stop() {
    if (!IsRunning()) {
        status_ = Status::kStopped;
        return;
    }

    LOG_TRACE("EventLoopThread::Stop, EventLoop = {}", (void*)loop_);
    assert(status_ == Status::kRunning);
    status_ = Status::kStopping;
    loop_->Quit();
}

NAMESPACE_END(net)
NAMESPACE_END(teno)
