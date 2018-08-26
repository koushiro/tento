//
// Created by koushiro on 8/26/18.
//

#pragma once

#include <tento/net/Poller.hpp>

#include <vector>

struct pollfd;

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// I/O Multiplexing with poll.
class PollPoller : public Poller {
public:
    explicit PollPoller(EventLoop* loop);
    ~PollPoller() override;

    Timestamp Poll(int timeoutMs, ChannelList* activeChannels) override;
    void UpdateChannel(Channel* channel) override;
    // Must be called after calling UpdateChannel (set NoneEvent).
    void RemoveChannel(Channel* channel) override;

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

private:
    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
