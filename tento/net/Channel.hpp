//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <functional>
#include <memory>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/base/Timestamp.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class EventLoop;

// A selectable I/O channel.
// Channel class doesn't own the file description.
// The file description could be a socketfd, an eventfd, a timerfd, or a signalfd.
class Channel : public NonCopyable {
public:
    typedef std::function<void()> EventCallback;
    typedef std::function<void(Timestamp)> ReadEventCallback;

public:
    Channel(EventLoop* loop, int fd);
    ~Channel();

    void SetReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
    void SetWriteCallback(EventCallback cb)    { writeCallback_ = std::move(cb); }
    void SetCloseCallback(EventCallback cb)    { closeCallback_ = std::move(cb); }
    void SetErrorCallback(EventCallback cb)    { errorCallback_ = std::move(cb); }

public:
    void HandleEvent(Timestamp receiveTime);
private:
    void handleEventWithGuard(Timestamp receiveTime);

public:
    //
    void Tie(const std::shared_ptr<void>&);

public:
    bool IsNoneEvent() const { return events_ == kNoneEvent; }
    bool IsReading()   const { return static_cast<bool>(events_ & kReadEvent); }
    bool IsWriting()   const { return static_cast<bool>(events_ & kWriteEvent); }

    void EnableReading()  { events_ |=  kReadEvent;  update(); }
    void DisableReading() { events_ &= ~kReadEvent;  update(); }
    void EnableWriting()  { events_ !=  kWriteEvent; update(); }
    void DisableWriting() { events_ &= ~kWriteEvent; update(); }
    void DisableAll()     { events_ =   kNoneEvent;  update(); }

private:
    void update();
public:
    void Remove();  // Must be called after calling DisableAll().

public:
    EventLoop* OwnerLoop()            { return ownerLoop_; }
    int  Fd()                   const { return fd_; }
    int  Events()               const { return events_; }
    void SetRevents(int revents)      { revents_ = revents; }

    // Used to judge whether the channel is a new one for Poller.
    // index == -1: New channel.
    // index > 0  : Existed channel.
    int  Index()                const { return index_; }
    void SetIndex(int index)          { index_  = index; }

public:     // For debug.
    std::string ReventsToString() const;
    std::string EventsToString() const;

private:    // For debug.
    static std::string eventsToString(int fd, int ev);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

private:
    EventLoop*  ownerLoop_; // The EventLoop to which the channel belongs.
    const int   fd_;        // File description, the channel isn't responsible for closing it.
    int         events_;    // Concerned events.
    int         revents_;   // The event returned by poll/epoll.
    int         index_;     // For PollPoller / EPollPoller.
    bool        logHup_;    // For POLLHUP/EPOLLHUP

    std::weak_ptr<void> tie_;
    bool                tied_;

    bool                eventHandling_; // The flag whether is currently in the event handling.
    ReadEventCallback   readCallback_;
    EventCallback       writeCallback_;
    EventCallback       closeCallback_;
    EventCallback       errorCallback_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
