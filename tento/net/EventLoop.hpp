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
#include "tento/base/Timestamp.hpp"
#include "tento/net/Alias.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;
class EPoller;
class TimerQueue;

class EventLoop : public NonCopyable {
public:
    EventLoop();
    ~EventLoop();

    /// Runs Loop forever.
    /// Must be called in the same thread as creation of the object.
    void Run();

    /// Quits Loop, thread safe.
    void Quit();

    /// Internal Usage (Used by Channel).
    void UpdateChannel(Channel* channel);   /// add or update channel in poller.
    void RemoveChannel(Channel* channel);   /// remove channel from poller.
    /// Internal Usage, wakeup I/O thread when polling (Used by ).
    void WakeUp();

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
    void AssertInLoopThread() { if (!IsInLoopThread()) { abortNotInLoopThread(); } }
    bool IsInLoopThread() const { return tid_ == std::this_thread::get_id(); }

private:
    void abortNotInLoopThread();
    void doPendingCallbacks();

private:
    static constexpr int kPollTimeMs = 10000;   /// 10 seconds;

private:
    std::thread::id tid_;

    bool looping_;
    std::atomic_bool quit_;
    std::atomic_bool eventHandling_;

    Timestamp pollReturnTime_;
    std::unique_ptr<EPoller> poller_;
    ChannelList activeChannels_;    /// active channels that returned from poller.

    std::unique_ptr<TimerQueue> timerQueue_;    /// Life time is managed by EventLoop.

    std::mutex mutex_;
    std::vector<Callback> pendingCallbacks_;    /// Guarded by mutex_.
    bool callingPendingCallbacks_;

    int eventFd_;                               /// Used by eventfd.
    std::unique_ptr<Channel> eventFdChannel_;   /// Life time is managed by EventLoop.
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
