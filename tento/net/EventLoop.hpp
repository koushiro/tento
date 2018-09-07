//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Thread.hpp"
#include "tento/base/Timestamp.hpp"
#include "tento/net/Alias.hpp"
#include "tento/net/ServerStatus.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;
class EPoller;
class TimerQueue;

class EventLoop : NonCopyable, public ServerStatus {
public:
    using Callback = std::function<void ()>;
    
    EventLoop();
    ~EventLoop();

    /// Runs I/O event loop forever.
    /// Must be called in the same thread as creation of the object.
    void Run();

    /// Quits event loop, thread safe.
    void Quit();

public:
    /// Runs callback immediately in the loop thread.
    /// It wakes up the loop, and run the cb.
    /// Safe to call from other threads.
    void RunInLoop(Callback cb);
    /// Queues callback in the loop thread.
    /// Runs after finish polling.
    /// Safe to call from other threads.
    void QueueInLoop(Callback cb);

    /// Safe to call from other threads.
    /// Runs timer callback (at timestamp/after delay time/every interval time).
    /// Returns unique timer id.
    TimerId RunAt(Timestamp time, TimerCallback cb);
    TimerId RunAfter(Duration delay, TimerCallback cb);
    TimerId RunEvery(Duration interval, TimerCallback cb);
    /// Safe to call from other threads.
    /// Cancel timer with given timerId.
    void CancelTimer(TimerId timerId);

public:
    void AssertInLoopThread();
    bool IsInLoopThread() const { return tid_ == thread_id(); }

    /// Internal Usage (Used by Channel).
    void UpdateChannel(Channel* channel);   /// add or update channel in poller.
    void RemoveChannel(Channel* channel);   /// remove channel from poller.

    /// Internal Usage, wakeup I/O thread when polling.
    void WakeUp();

private:
    void abortNotInLoopThread();
    void doPendingCallbacks();

private:
    static constexpr int kPollTimeMs = 10000;   /// 10 seconds;

private:
    size_t tid_;    /// consistent with spdlog thread id format, see OS.h.

    std::atomic_bool eventHandling_;

    Timestamp pollReturnTime_;
    std::unique_ptr<EPoller> poller_;
    ChannelList activeChannels_;    /// active channels that returned from poller.

    std::unique_ptr<TimerQueue> timerQueue_;    /// Life time is managed by EventLoop.

    std::mutex mutex_;
    std::vector<Callback> pendingCallbacks_;    /// Guarded by mutex_.
    std::atomic_bool callingPendingCallbacks_;

    int eventFd_;                               /// Used by eventfd.
    std::unique_ptr<Channel> eventFdChannel_;   /// Life time is managed by EventLoop.
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
