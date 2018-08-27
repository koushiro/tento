//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <atomic>
#include <thread>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Timestamp.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;
class EPoller;

using ChannelList = std::vector<Channel*>;

class EventLoop : public NonCopyable {
public:
    EventLoop();
    ~EventLoop();

    void Loop();
    void Quit();

    void UpdateChannel(Channel* channel);   // add or update channel in poller.
    void RemoveChannel(Channel* channel);   // remove channel from poller.

public:
    void AssertInLoopThread() {
        if (!IsInLoopThread()) {
            abortNotInLoopThread();
        }
    }

    bool IsInLoopThread() const { return tid_ == std::this_thread::get_id(); }

private:
    void abortNotInLoopThread();

private:
    static const int kPollTimeMs;

private:
    std::thread::id tid_;

    std::atomic_bool looping_;
    std::atomic_bool quit_;
    std::atomic_bool eventHandling_;

    Timestamp pollReturnTime_;
    std::unique_ptr<EPoller> poller_;

    ChannelList activeChannels_;        // active channels that returned from poller.
    Channel* currentActiveChannel_;     // the active channel currently being processed.
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
