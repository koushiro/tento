//
// Created by koushiro on 9/5/18.
//

#pragma once

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/ServerStatus.hpp"
#include "tento/net/EventLoopThread.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class EventLoopThreadPool : NonCopyable, public ServerStatus {
public:
    explicit EventLoopThreadPool(EventLoop* baseLoop, uint32_t numThread = 0);
    ~EventLoopThreadPool();

    void Start();
    /// @brief Stop all working thread. If you forget to call this method,
    /// it will be invoked automatically in the destructor.
    /// @note DO NOT call this method from any of the working thread.
    void Stop();

public:
    EventLoop* GetNextLoop();
    EventLoop* GetNextLoopWithHash(uint64_t hash);

private:
    EventLoop* baseLoop_;
    uint32_t numThread_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::atomic_uint64_t next_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
