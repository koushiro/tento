//
// Created by koushiro on 8/23/18.
//

#include "tento/net/EPoller.hpp"

#include "tento/base/Logger.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/EventLoop.hpp"

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

void EPoller::Poll(int timeoutMs, ChannelList* activeChannels) {
    int numReadyFd = epoll_wait(epfd_,
                               events_.data(),
                               static_cast<int>(events_.size()),
                               timeoutMs);

    if (numReadyFd == -1) {
        auto errorCode = errno;
        LOG_ERROR("EPoller::Poll - epoll_wait() failed, "
                  "an error '{}' occurred", strerror(errorCode));
    } else if (numReadyFd == 0) {
        LOG_TRACE("Nothing happened", "");
    } else {
        LOG_TRACE("Events happened, num of ready fd = {}", numReadyFd);
        for (int i = 0; i < numReadyFd; ++i) {
            auto channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->SetRevents(events_[i].events);
            activeChannels->push_back(channel);
        }
        if (numReadyFd == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    }
}

void EPoller::UpdateChannel(Channel* channel) {
    assert(ownerLoop_->IsInLoopThread());
    auto status = channel->Status();
    LOG_TRACE("UpdateChannel, fd = {}, events = {}, status = {}",
              channel->Fd(), channel->EventsToString(), status);

    switch (status) {
        case ChannelStatus::kAdded:
            // it's an Added channel, update the existed channel in the events_.
            if (channel->IsNoneEvent()) {
                control(EPOLL_CTL_DEL, channel);
                channel->SetStatus(ChannelStatus::kDeleted);
            } else {
                control(EPOLL_CTL_MOD, channel);
            }
            break;

        case ChannelStatus::kNew:
        case ChannelStatus::kDeleted:
            // it's a New channel, add the new channel to events_.
            control(EPOLL_CTL_ADD, channel);
            channel->SetStatus(ChannelStatus::kAdded);
            break;
    }
}

void EPoller::RemoveChannel(Channel* channel) {
    assert(ownerLoop_->IsInLoopThread());
    LOG_TRACE("RemoveChannel, fd = {}", channel->Fd());
    assert(channel->IsNoneEvent());

    auto status = channel->Status();
    assert(status == ChannelStatus::kAdded || status == ChannelStatus::kDeleted);

    if (status == ChannelStatus::kAdded) {
        control(EPOLL_CTL_DEL, channel);
    }
    channel->SetStatus(ChannelStatus::kNew);
}

void EPoller::control(int op, Channel* channel) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = static_cast<uint32_t>(channel->Events());
    event.data.ptr = channel;

    LOG_TRACE("control operation = {} (ADD:1, DEL:2, MOD:3), "
              "fd = {}, event = {}",
              op, channel->Fd(), channel->EventsToString());
    int ret = epoll_ctl(epfd_, op, channel->Fd(), &event);
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("EPoller::control - epoll_ctl() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

NAMESPACE_END(net)
NAMESPACE_END(tento)