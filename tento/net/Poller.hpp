//
// Created by koushiro on 8/26/18.
//

#pragma once

#include <tento/base/Common.hpp>
#include <tento/base/NonCopyable.hpp>
#include <tento/base/Timestamp.hpp>
#include <tento/net/EventLoop.hpp>

#include <vector>
#include <map>

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;

// Base class for I/O Multiplexing.
class Poller : public NonCopyable {
public:
    typedef std::vector<Channel*> ChannelList;

public:
    explicit Poller(EventLoop* loop) : ownerLoop_(loop) {}
    virtual ~Poller() = default;

    // Must be called in the loop thread.
    virtual Timestamp Poll(int timeoutMs, ChannelList* activeChannels) = 0;

    // Must be called in the loop thread.
    virtual void UpdateChannel(Channel* channel) = 0;

    // Must be called in the loop thread.
    virtual void RemoveChannel(Channel* channel) = 0;

public:
    static Poller* NewDefaultPoller(EventLoop* loop);

protected:
    typedef std::map<int, Channel*> ChannelMap;  // key - file description, value - channel.
    ChannelMap channels_;

private:
    EventLoop* ownerLoop_;  // The EventLoop to which the poller belongs.
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
