//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <sys/epoll.h>

#include <map>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/Alias.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;
class EventLoop;

/// I/O Multiplexing with epoll.
class EPoller : NonCopyable {
public:
    explicit EPoller(EventLoop* loop);
    ~EPoller();

    void Poll(int timeoutMs, ChannelList* activeChannels);

    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);

private:
    void control(int op, Channel* channel);

private:
    static constexpr size_t kInitEventListSize = 16;

    EventLoop* ownerLoop_;
    int epfd_;
    std::vector<struct epoll_event> events_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
