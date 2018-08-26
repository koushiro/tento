//
// Created by koushiro on 8/26/18.
//

#include "tento/net/PollPoller.hpp"

#include <sys/poll.h>

#include <algorithm>

#include "tento/base/Logger.hpp"
#include "tento/net/Channel.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

PollPoller::PollPoller(EventLoop* loop)
    : Poller(loop)
{
}

PollPoller::~PollPoller() = default;

Timestamp PollPoller::Poll(int timeoutMs, ChannelList* activeChannels) {
    // pollfds is an input and output parameter.
    int numEvents = poll(pollfds_.data(), pollfds_.size(), timeoutMs);
    Timestamp now = Timestamp::Now();
    if (numEvents > 0) {
        LOG_TRACE("Events happened, num of events = {}", numEvents);
        fillActiveChannels(numEvents, activeChannels);
    } else if (numEvents == 0) {
        LOG_TRACE("Nothing happened, num of events = {}", numEvents);
    } else {
        LOG_ERROR("PollPoller::Poll(), num of events < 0");
    }
    return now;
}

void PollPoller::fillActiveChannels(int numEvents,
                                    ChannelList* activeChannels) const
{
    assert(numEvents > 0);
    for (auto pfd = pollfds_.begin();
         numEvents > 0 && pfd != pollfds_.end(); ++pfd)
    {
        if (pfd->revents > 0) {
            --numEvents;
            auto ch = channels_.find(pfd->fd);
            assert(ch != channels_.end());
            Channel* channel = ch->second;
            assert(channel->Fd() == pfd->fd);
            channel->SetRevents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void PollPoller::UpdateChannel(Channel* channel) {
    LOG_TRACE("UpdateChannel, fd = {}, events = {}",
              channel->Fd(), channel->Events());
    if (channel->Index() < 0) {
        // New channel, add the new channel to pollfds_.
        assert(channels_.find(channel->Fd()) == channels_.end());
        struct pollfd pfd;
        pfd.fd = channel->Fd();
        pfd.events = static_cast<short>(channel->Events());
        pfd.revents = 0;
        pollfds_.push_back(pfd);

        channel->SetIndex(static_cast<int>(pollfds_.size() - 1));
        channels_[pfd.fd] = channel;
    } else {
        // Existed channel, update the existed channel in the pollfds_.
        assert(channels_.find(channel->Fd()) != channels_.end());
        assert(channels_[channel->Fd()] == channel);
        int index = channel->Index();
        assert(index > 0 && index < pollfds_.size());

        auto& pfd = pollfds_[index];
        assert(pfd.fd == channel->Fd() || pfd.fd == -1);
        pfd.events = static_cast<short>(channel->Events());
        pfd.revents = 0;

        // Disable all events of this channel temporarily,
        // but don't remove the channel from the poller.
        if (channel->IsNoneEvent()) {
            // just ignore this pollfd, POLLNVAL: fd value is invalid.
//            pfd.fd = -1;
            pfd.fd = -channel->Fd() - 1;    // Optimization for RemoveChannel.
        }
    }
}

void PollPoller::RemoveChannel(Channel* channel) {
    LOG_TRACE("RemoveChannel, fd = {}", channel->Fd());
    assert(channels_.find(channel->Fd()) != channels_.end());
    assert(channels_[channel->Fd()] == channel);
    // Called UpdateChannel (set NoneEvent) before RemoveChannel.
    assert(channel->IsNoneEvent());
    int index = channel->Index();
    assert(index > 0 && index < pollfds_.size());

    auto& pfd = pollfds_[index];
    assert(pfd.fd < 0 && pfd.events == channel->Events());
    size_t n = channels_.erase(channel->Fd());
    assert(n == 1);

    // If the fd of the channel to be removed is not the last pollfd of pollfds_,
    // exchange the pollfd to be deleted with the last pollfd, and pop_back.
    if (index != pollfds_.size() - 1) {
        //  pollfds_ vector.
        // +---------------------------------------------------+
        // |begin|     |     |  ...  |index|  ...  |     |back |  end
        // +---------------------------------------------------+
        int backFd = pollfds_.back().fd;
        if (backFd < 0) {
            // The fd of NoneEvent channel need to be recovery.
            backFd = -backFd - 1;
        }
        channels_[backFd]->SetIndex(index);
        std::iter_swap(pollfds_.begin() + index, pollfds_.end() - 1);
    }
    pollfds_.pop_back();
}

NAMESPACE_END(net)
NAMESPACE_END(tento)