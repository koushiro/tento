//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <sys/epoll.h>

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

enum class ChannelStatus {
    kNew = -1,      /// New channel, initial status.
    kAdded = 1,     /// Added channel.
    kDeleted = 2,   /// Deleted channel.
};

inline std::ostream& operator<<(std::ostream& os, ChannelStatus status) {
    switch (status) {
        case ChannelStatus::kNew:     return os << "kNew";
        case ChannelStatus::kAdded:   return os << "kAdded";
        case ChannelStatus::kDeleted: return os << "kDeleted";
    }
}

class EventLoop;

/// A selectable I/O channel.
/// Channel class doesn't own the file description.
/// The file description could be a socketfd, an eventfd, a timerfd, or a signalfd.
class Channel : NonCopyable {
public:
    using EventCallback = std::function<void ()>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void HandleEvent();

    void SetReadCallback(EventCallback cb)  { readCallback_  = std::move(cb); }
    void SetWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void SetErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }
    void SetCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }

    bool IsNoneEvent() const { return events_ == kNone; }
    bool IsReadable()  const { return static_cast<bool>(events_ & kRead); }
    bool IsWritable()  const { return static_cast<bool>(events_ & kWrite); }

    void EnableReadEvent()   { events_ |=  kRead;  update(); }
    void DisableReadEvent()  { events_ &= ~kRead;  update(); }
    void EnableWriteEvent()  { events_ |=  kWrite; update(); }
    void DisableWriteEvent() { events_ &= ~kWrite; update(); }
    void DisableAllEvents()  { events_ =   kNone;  update(); }

    void Remove();  // Must call DisableAll() before calling Remove().

private:
    void update();

public:
    // Getter and setter
    EventLoop* OwnerLoop()                { return ownerLoop_; }
    int  Fd()                       const { return fd_; }
    int  Events()                   const { return events_; }
    void SetRevents(int revents)          { revents_ = revents; }
    ChannelStatus  Status()         const { return status_; }
    void SetStatus(ChannelStatus status)  { status_  = status; }

    // For debug.
    std::string EventsToString() const;

private:
    enum EventStatus {
        kNone = 0,
        kRead = EPOLLIN | EPOLLPRI,
        kWrite = EPOLLOUT,
    };

    EventLoop* ownerLoop_; /// The EventLoop to which the channel belongs.
    const int  fd_;        /// File description, the channel isn't responsible for closing it.

    int events_;           /// User's Concerned events status (readable, writable, none).
    int revents_;          /// The event status returned by epoll.
    ChannelStatus status_; /// For EPoller.

    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
