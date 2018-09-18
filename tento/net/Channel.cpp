//
// Created by koushiro on 8/23/18.
//

#include "tento/net/Channel.hpp"

#include <cassert>
#include <sstream>
#include <string>

#include "tento/base/Logger.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

Channel::Channel(EventLoop* loop, int fd)
    : ownerLoop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      status_(ChannelStatus::kNew)
{
}

Channel::~Channel() {

}

void Channel::HandleEvent() {
    // see man `epoll_ctl`
    // TODO: specific comment
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN) && closeCallback_) {
        closeCallback_();
    }
    // TODO: specific comment
    if ((revents_ & EPOLLERR) && errorCallback_) {
        errorCallback_();
    }
    // TODO: specific comment
    if ((revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) && readCallback_) {
        readCallback_();
    }
    // 当某次write写满发送缓冲区，errno为EAGAIN;
    // 对端接受了部分数据，发送缓冲区又可写了，触发EPOLLOUT
    if ((revents_ & EPOLLOUT) && writeCallback_) {
        writeCallback_();
    }
}

void Channel::update() {
    ownerLoop_->UpdateChannel(this);
}

void Channel::Remove() {
    assert(IsNoneEvent());
    ownerLoop_->RemoveChannel(this);
}

std::string Channel::EventsToString() const {
    std::ostringstream oss;
    if (events_ & EPOLLIN)      oss << "IN ";
    if (events_ & EPOLLPRI)     oss << "PRI ";
    if (events_ & EPOLLOUT)     oss << "OUT ";
    if (events_ & EPOLLHUP)     oss << "HUP ";
    if (events_ & EPOLLRDHUP)   oss << "RDHUP ";
    if (events_ & EPOLLERR)     oss << "ERR ";
    return oss.str();
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
