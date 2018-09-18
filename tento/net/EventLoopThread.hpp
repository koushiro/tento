//
// Created by koushiro on 8/23/18.
//

#pragma once

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Thread.hpp"
#include "tento/net/EventLoop.hpp"
#include "tento/net/ServerStatus.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// RAII Wrapper of EventLoop.
class EventLoopThread : NonCopyable, public ServerStatus {
public:
    EventLoopThread();
    ~EventLoopThread();

    /// @brief Stop the working thread manually.
    /// You don't have to call this method, 
    /// because it will be invoked automatically in the destructor.
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
