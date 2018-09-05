//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <sys/epoll.h>

#include <map>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// I/O Multiplexing with epoll.
class EPoller : NonCopyable {
public:
    explicit EPoller(EventLoop* loop);
    ~EPoller();

    Timestamp Poll(int timeoutMs, ChannelList* activeChannels);
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);

private:
    void epollControl(int op, Channel* channel);

public:
    static constexpr int kNew = -1;
    static constexpr int kAdded = 1;
    static constexpr int kDeleted = 2;

private:
    static constexpr size_t kInitEventListSize = 16;

    EventLoop* ownerLoop_;
    int epfd_;
    std::vector<struct epoll_event> events_;
    // key - file description, value - channel.
    std::map<int, Channel*> channels_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
