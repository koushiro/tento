//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EventLoopThread.hpp"

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

EventLoopThread::EventLoopThread()
    : loop_(nullptr),
      startLatch_(1),
      stopLatch_(1)
{
    LOG_TRACE("EventLoopThread::EventLoopThread", "");
}

EventLoopThread::~EventLoopThread() {
    LOG_TRACE("EventLoopThread::~EventLoopThread", "");
    Stop();
}

EventLoop* EventLoopThread::Start() {
    LOG_TRACE("EventLoopThread::Start", "");
    status_ = Status::kStarting;

    assert(thread_ == nullptr);
    thread_ = std::make_unique<std::thread>([&]() {
        status_ = Status::kRunning;
        tid_ = thread_id();
        name_ = fmt::format("thread-{}", tid_);

        EventLoop loop;
        loop_ = &loop;

        startLatch_.CountDown();
        loop_->Run();

        LOG_TRACE("EventLoopThread::Start, EventLoop = {} stopped.", (void*)loop_);
        status_ = Status::kStopped;
        loop_ = nullptr;
        stopLatch_.CountDown();
    });
    startLatch_.Wait();

    assert(loop_ != nullptr);
    return loop_;
}

void EventLoopThread::Stop() {
    if (!IsRunning()) {
        status_ = Status::kStopped;
        Join();
        return;
    }

    LOG_TRACE("EventLoopThread::Stop, EventLoop = {}", (void*)loop_);
    assert(status_ == Status::kRunning);
    status_ = Status::kStopping;
    loop_->Quit();

    stopLatch_.Wait();

    Join();
}

void EventLoopThread::Join() {
    assert(IsStopped());
    if (thread_ && thread_->joinable()) {
        LOG_TRACE("EventLoopThread::Join, join thread {}", tid_);
        try {
            thread_->join();
        } catch (const std::system_error& error) {
            LOG_ERROR("EventLoopThread::Join, "
                      "Caught a system error: {}, code = {}",
                      error.what(), error.code());
        }
    } else {
        LOG_TRACE("EventLoopThread::Join, thread {} had joined", tid_);
    }
}

NAMESPACE_END(net)
NAMESPACE_END(teno)
