//
// Created by koushiro on 8/23/18.
//

#include "tento/net/Channel.hpp"

#include <sys/poll.h>

#include <cassert>
#include <sstream>
#include <string>

#include "tento/base/Logger.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
    : ownerLoop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      logHup_(true),
      tied_(false),
      eventHandling_(false)
{
}

Channel::~Channel() {
    assert(!eventHandling_);
}

void Channel::HandleEvent(tento::Timestamp timestamp) {

    // call handleEventWithGuard()
    handleEventWithGuard(timestamp);
}

void Channel::handleEventWithGuard(Timestamp receiveTime) {
    eventHandling_  = true;
    if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
        if (logHup_) {
            LOG_WARN("fd = {} Channel::handleEventWithGuard() POLLHUP", fd_);
        }
        if (closeCallback_) closeCallback_();
    }
    if (revents_ & POLLNVAL) {
        LOG_WARN("fd = {} Channel::handleEventWithGuard() POLLNVAL", fd_);
    }
    if (revents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback_) errorCallback_();
    }
    if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback_) readCallback_(receiveTime);
    }
    if (revents_ & POLLOUT) {
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
    if (ev & POLLIN)
        oss << "IN ";
    if (ev & POLLPRI)
        oss << "PRI ";
    if (ev & POLLOUT)
        oss << "OUT ";
    if (ev & POLLHUP)
        oss << "HUP ";
    if (ev & POLLRDHUP)
        oss << "RDHUP ";
    if (ev & POLLERR)
        oss << "ERR ";
    if (ev & POLLNVAL)
        oss << "NVAL ";
    return oss.str().c_str();
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
