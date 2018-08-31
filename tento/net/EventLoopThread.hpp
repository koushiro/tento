//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <mutex>
#include <thread>

#include "tento/base/Common.hpp"
#include "tento/base/CountDownLatch.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class EventLoopThread : public NonCopyable {
public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* StartLoop();

private:
    EventLoop* loop_;
    bool started_;
    std::thread thread_;
    CountDownLatch latch_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
