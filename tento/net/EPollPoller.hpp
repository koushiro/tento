//
// Created by koushiro on 8/23/18.
//

#pragma once

#include "tento/net/Poller.hpp"

struct epoll_event;

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// I/O Multiplexing with epoll.
class EPollPoller : public Poller {
public:
    explicit EPollPoller(EventLoop* loop);
    ~EPollPoller() override;

    Timestamp Poll(int timeoutMs, ChannelList* activeChannels) override;
    void UpdateChannel(Channel* channel) override;
    void RemoveChannel(Channel* channel) override;

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
    void epollControl(int operation, Channel* channel);

private:
    static const char* operationToString(int op);

private:
    static const int kNew;
    static const int kAdded;
    static const int kDeleted;
    static const size_t kInitEventListSize;

private:
    typedef std::vector<struct epoll_event> EventList;

    int epollfd_;
    EventList events_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
