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
class EPoller : public NonCopyable {
public:
    explicit EPoller(EventLoop* loop);
    ~EPoller();

    Timestamp Poll(int timeoutMs, ChannelList* activeChannels);
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);

private:
    void epollControl(int operation, Channel* channel);

public:
    static const int kNew;
    static const int kAdded;
    static const int kDeleted;

private:
    static const size_t kInitEventListSize;

    EventLoop* ownerLoop_;
    int epfd_;
    std::vector<struct epoll_event> events_;
    // key - file description, value - channel.
    std::map<int, Channel*> channels_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
