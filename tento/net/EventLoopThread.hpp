//
// Created by koushiro on 8/23/18.
//

#pragma once

#include "tento/base/Common.hpp"
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

    /// @brief Stop the working thread manually.
    /// If you forget to call this method,
    /// it will be invoked automatically in the destructor.
    /// @note DO NOT call this method from any of the working thread.
    void Stop();

public:
    const size_t ThreadId()   const { return tid_; }
    EventLoop* GetLoop()      const { return loop_; }

private:
    EventLoop* loop_;

    size_t tid_;
    std::unique_ptr<Thread> thread_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
