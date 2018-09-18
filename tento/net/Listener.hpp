//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <functional>
#include <memory>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/Socket.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;
class EventLoop;

/// Tcp Listener.
class Listener : NonCopyable {
public:
    using NewConnectionCallback= std::function<
        void (Socket connSock, const SockAddr& remoteAddr)
    >;

    Listener(EventLoop* loop, const SockAddr& listenAddr);
    ~Listener();

    /// socket listen.
    void Listen(int n = SOMAXCONN);

    /// non-blocking accept (using accept4).
    void Accept();

    /// @brief Stop Listener manually.
    /// You don't have to call this method,
    /// because it will be invoked automatically in the destructor.
    /// @note DO NOT call this method from other thread.
    void Stop();

    void SetNewConnectionCallback(NewConnectionCallback cb) {
        newConnCallback_ = std::move(cb);
    }

    bool IsListening() const { return listening_; }

private:
    EventLoop* loop_;
    Socket listenSocket_;     /// The listening socket.
    SockAddr listenAddr_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnCallback_;
    bool listening_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
