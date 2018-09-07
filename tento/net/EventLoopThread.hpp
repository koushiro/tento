//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <mutex>
#include <thread>

#include "tento/base/Common.hpp"
#include "tento/base/CountDownLatch.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Thread.hpp"
#include "tento/net/ServerStatus.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// Wrapper of EventLoop.
class EventLoopThread : NonCopyable, public ServerStatus {
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* Start();
    /// @brief Stop and Join the working thread. If you forget to call this method,
    /// it will be invoked automatically in the destructor.
    /// @note DO NOT call this method from any of the working thread.
    void Stop();

public:
    const size_t ThreadId()   const { return tid_; }
    const std::string& Name() const { return name_; }
    EventLoop* Loop()         const { return loop_; }

private:
    void Join();

private:
    EventLoop* loop_;
    CountDownLatch startLatch_;
    CountDownLatch stopLatch_;

    size_t tid_;
    std::string name_;
    std::unique_ptr<std::thread> thread_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
