//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include <sys/epoll.h>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Timestamp.hpp"
#include "tento/net/Alias.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// A selectable I/O channel.
/// Channel class doesn't own the file description.
/// The file description could be a socketfd, an eventfd, a timerfd, or a signalfd.
class Channel : NonCopyable {
public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void SetReadCallback(EventCallback cb)  { readCallback_ = std::move(cb); }
    void SetWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
    void SetCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
    void SetErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

public:
    void HandleEvent(Timestamp when);
private:
    void handleEventWithGuard();

public:
    ///
//    void Tie(const std::shared_ptr<void>&);

public:
    bool IsNoneEvent() const { return events_ == kNoneEvent; }
    bool IsReading()   const { return static_cast<bool>(events_ & kReadEvent); }
    bool IsWriting()   const { return static_cast<bool>(events_ & kWriteEvent); }

    void EnableReadEvent()   { events_ |=  kReadEvent;  update(); }
    void DisableReadEvent()  { events_ &= ~kReadEvent;  update(); }
    void EnableWriteEvent()  { events_ |=  kWriteEvent; update(); }
    void DisableWriteEvent() { events_ &= ~kWriteEvent; update(); }
    void DisableAllEvents()  { events_ =   kNoneEvent;  update(); }

private:
    void update();
public:
    void Remove();  // Must be called after calling DisableAll().

public:
    EventLoop* OwnerLoop()            { return ownerLoop_; }
    int  Fd()                   const { return fd_; }
    int  Events()               const { return events_; }
    void SetRevents(int revents)      { revents_ = revents; }

    /// Used to judge whether the channel is a new one for Poller.
    /// index == -1: New channel.
    /// index > 0  : Existed channel.
    int  Status()                const { return status_; }
    void SetStatus(int status)         { status_  = status; }

    /// For debug.
public:
    std::string ReventsToString() const;
    std::string EventsToString() const;
private:
    static std::string eventsToString(int fd, int ev);

private:
    static constexpr int kNoneEvent = 0;
    static constexpr int kReadEvent = EPOLLIN | EPOLLPRI;
    static constexpr int kWriteEvent = EPOLLIN | EPOLLPRI;

private:
    EventLoop*  ownerLoop_; /// The EventLoop to which the channel belongs.
    const int   fd_;        /// File description, the channel isn't responsible for closing it.
    int         events_;    /// Concerned events.
    int         revents_;   /// The event returned by epoll.
    int         status_;    /// For EPoller.
    bool        logHup_;    /// For EPOLLHUP

//    std::weak_ptr<void> tie_;
//    bool                tied_;

    bool          eventHandling_; /// The flag whether is currently in the event handling.
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
