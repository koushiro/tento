//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EPollPoller.hpp"

#include <sys/epoll.h>
#include <unistd.h>

#include "tento/base/Logger.hpp"
#include "tento/net/Channel.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

const int EPollPoller::kNew = -1;
const int EPollPoller::kAdded = 1;
const int EPollPoller::kDeleted = 2;
const size_t EPollPoller::kInitEventListSize = 16;

EPollPoller::EPollPoller(EventLoop* loop)
    : Poller(loop),
      epollfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if (epollfd_ <= 0) {
        LOG_ERROR("EPollPoller::EPollPoller epoll_create1 failed");
    }
}

EPollPoller::~EPollPoller() {
    close(epollfd_);
}

Timestamp EPollPoller::Poll(int timeoutMs, ChannelList* activeChannels) {
    int numEvents = epoll_wait(epollfd_,
                               events_.data(),
                               static_cast<int>(events_.size()),
                               timeoutMs);
    Timestamp now = Timestamp::Now();
    if (numEvents > 0) {
        LOG_TRACE("Events happened, num of events = {}", numEvents);
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        LOG_TRACE("Nothing happened, num of events = {}", numEvents);
    } else {
        LOG_ERROR("EPollPoller::Poll(), num of events < 0");
    }
    return now;
}

void EPollPoller::fillActiveChannels(int numEvents,
                                     ChannelList* activeChannels) const
{
    assert(numEvents > 0);
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        int fd = channel->Fd();
        auto it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);
        channel->SetRevents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::UpdateChannel(Channel* channel) {
    int index = channel->Index();
    LOG_TRACE("UpdateChannel, fd = {}, events = {}, index = {}",
              channel->Fd(), channel->Events(), index);
    if (index == kNew || kDeleted) {
        // New channel, add the new channel to events_ with EPOLL_CTL_ADD.
        if (index == kNew) {
            assert(channels_.find(channel->Fd()) == channels_.end());
            channels_[channel->Fd()] = channel;
        } else {
            assert(channels_.find(channel->Fd()) != channels_.end());
            assert(channels_[channel->Fd()] == channel);
        }
        channel->SetIndex(kAdded);
        epollControl(EPOLL_CTL_ADD, channel);
    } else {
        // Existed channel, update the existed channel in the events_.
        assert(channels_.find(channel->Fd()) != channels_.end());
        assert(channels_[channel->Fd()] == channel);
        assert(index == kAdded);
        if (channel->IsNoneEvent()) {
            epollControl(EPOLL_CTL_DEL, channel);
            channel->SetIndex(kDeleted);
        } else {
            epollControl(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::RemoveChannel(Channel* channel) {
    LOG_TRACE("RemoveChannel, fd = {}", channel->Fd());
    assert(channels_.find(channel->Fd()) != channels_.end());
    assert(channels_[channel->Fd()] == channel);
    // Called UpdateChannel (set NoneEvent) before RemoveChannel.
    assert(channel->IsNoneEvent());
    int index = channel->Index();
    assert(index == kAdded || index == kDeleted);
    size_t num = channels_.erase(channel->Fd());
    assert(num == 1);

    if (index == kAdded) {
        epollControl(EPOLL_CTL_DEL, channel);
    }
    channel->SetIndex(kNew);
}

void EPollPoller::epollControl(int operation, Channel* channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = static_cast<uint32_t>(channel->Events());
    event.data.ptr = channel;
    LOG_TRACE("epollControl operation = {}, fd = {}, event = {}",
              operationToString(operation),
              channel->Fd(),
              channel->EventsToString());
    if (epoll_ctl(epollfd_, operation, channel->Fd(), &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR("epollControl operation = {}, fd = {}",
                      operationToString(operation), channel->Fd());
        } else {
            LOG_FATAL("epollControl operation = {}, fd = {}",
                      operationToString(operation), channel->Fd());
        }
    }
}

const char* EPollPoller::operationToString(int op) {
    switch (op) {
        case EPOLL_CTL_ADD: return "ADD";
        case EPOLL_CTL_DEL: return "DEL";
        case EPOLL_CTL_MOD: return "MOD";
        default:            return "Unknown Operation";
    }
}

NAMESPACE_END(net)
NAMESPACE_END(tento)