//
// Created by koushiro on 8/23/18.
//

#include "tento/net/Channel.hpp"

#include <sys/epoll.h>

#include <cassert>
#include <sstream>
#include <string>

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : ownerLoop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      status_(-1),
      logHup_(true),
      tied_(false),
      eventHandling_(false)
{
}

Channel::~Channel() {
    assert(!eventHandling_);
}

void Channel::HandleEvent(Timestamp when) {

    // call handleEventWithGuard()
    handleEventWithGuard();
}

void Channel::handleEventWithGuard() {
    eventHandling_  = true;
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (logHup_) {
            LOG_WARN("fd = {} Channel::handleEventWithGuard() EPOLLHUP", fd_);
        }
        if (closeCallback_) closeCallback_();
    }
    if (revents_ & EPOLLERR) {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (readCallback_) readCallback_();
    }
    if (revents_ & EPOLLOUT) {
        if (writeCallback_) writeCallback_();
    }
    eventHandling_ = false;
}

void Channel::update() {
    ownerLoop_->UpdateChannel(this);
}

void Channel::Remove() {
    assert(IsNoneEvent());
    ownerLoop_->RemoveChannel(this);
}

//
void Channel::Tie(const std::shared_ptr<void>& obj) {
    tie_ = obj;
    tied_ = true;
}

std::string Channel::ReventsToString() const {
    return eventsToString(fd_, revents_);
}

std::string Channel::EventsToString() const {
    return eventsToString(fd_, events_);
}

std::string Channel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ": ";
    if (ev & EPOLLIN)
        oss << "IN ";
    if (ev & EPOLLPRI)
        oss << "PRI ";
    if (ev & EPOLLOUT)
        oss << "OUT ";
    if (ev & EPOLLHUP)
        oss << "HUP ";
    if (ev & EPOLLRDHUP)
        oss << "RDHUP ";
    if (ev & EPOLLERR)
        oss << "ERR ";
    return oss.str().c_str();
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
