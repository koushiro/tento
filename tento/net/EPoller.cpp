//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EPoller.hpp"

#include <unistd.h>

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

EPoller::EPoller(EventLoop* loop)
    : ownerLoop_(loop),
      epfd_(epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if (epfd_ == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("EPoller::EPoller - epoll_create1() failed, "
                     "an error '{}' occurred", strerror(errorCode));
        abort();
    }
}

EPoller::~EPoller() {
    close(epfd_);
}

Timestamp EPoller::Poll(int timeoutMs, ChannelList* activeChannels) {
    int numReadyFd = epoll_wait(epfd_,
                               events_.data(),
                               static_cast<int>(events_.size()),
                               timeoutMs);
    Timestamp now = Timestamp::Now();
    if (numReadyFd == -1) {
        auto errorCode = errno;
        LOG_ERROR("EPoller::Poll - epoll_wait() failed, "
                  "an error '{}' occurred", strerror(errorCode));
    } else if (numReadyFd == 0) {
        LOG_TRACE("Nothing happened", numReadyFd == 0);
    } else {
        LOG_TRACE("Events happened, num of events = {}", numReadyFd);
        for (int i = 0; i < numReadyFd; ++i) {
            auto channel = static_cast<Channel*>(events_[i].data.ptr);
            auto it = channels_.find(channel->Fd());
            assert(it != channels_.end());
            assert(it->second == channel);
            channel->SetRevents(events_[i].events);
            activeChannels->push_back(channel);
        }
        if (numReadyFd == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    }
    return now;
}

void EPoller::UpdateChannel(Channel* channel) {
    assert(ownerLoop_->IsInLoopThread());
    int status = channel->Status();
    LOG_TRACE("UpdateChannel, fd = {}, events = {}, status = {}",
              channel->Fd(), channel->EventsToString(), status);
    if (status == kNew || status == kDeleted) {
        // New channel, add the new channel to events_ with EPOLL_CTL_ADD.
        if (status == kNew) {
            assert(channels_.find(channel->Fd()) == channels_.end());
            channels_[channel->Fd()] = channel;
        } else {
            assert(channels_.find(channel->Fd()) != channels_.end());
            assert(channels_[channel->Fd()] == channel);
        }
        channel->SetStatus(kAdded);
        epollControl(EPOLL_CTL_ADD, channel);
    } else {
        // Existed channel, update the existed channel in the events_.
        assert(channels_.find(channel->Fd()) != channels_.end());
        assert(channels_[channel->Fd()] == channel);
        assert(status == kAdded);
        if (channel->IsNoneEvent()) {
            epollControl(EPOLL_CTL_DEL, channel);
            channel->SetStatus(kDeleted);
        } else {
            epollControl(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPoller::RemoveChannel(Channel* channel) {
    assert(ownerLoop_->IsInLoopThread());
    LOG_TRACE("RemoveChannel, fd = {}", channel->Fd());
    assert(channels_.find(channel->Fd()) != channels_.end());
    assert(channels_[channel->Fd()] == channel);
    // Called UpdateChannel (set NoneEvent) before RemoveChannel.
    assert(channel->IsNoneEvent());
    int status = channel->Status();
    assert(status == kAdded || status == kDeleted);
    size_t num = channels_.erase(channel->Fd());
    assert(num == 1);

    if (status == kAdded) {
        epollControl(EPOLL_CTL_DEL, channel);
    }
    channel->SetStatus(kNew);
}

void EPoller::epollControl(int op, Channel* channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = static_cast<uint32_t>(channel->Events());
    event.data.ptr = channel;
    LOG_TRACE("epollControl operation = {} (ADD:1, DEL:2, MOD:3), "
              "fd = {}, event = {}",
              op, channel->Fd(), channel->EventsToString());
    int ret = epoll_ctl(epfd_, op, channel->Fd(), &event);
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("EPoller::epollControl - epoll_ctl() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

NAMESPACE_END(net)
NAMESPACE_END(tento)