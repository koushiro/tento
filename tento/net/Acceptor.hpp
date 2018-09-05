//
// Created by koushiro on 8/23/18.
//

#pragma once

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/Socket.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class EventLoop;

class Acceptor : NonCopyable {
public:
    using NewConnectionCallback
        = std::function<void (Socket sock, const SockAddr& peerAddr)>;

    Acceptor(EventLoop* loop, const SockAddr& listenAddr);
    ~Acceptor();

    void Listen();

    void SetNewConnectionCallback(NewConnectionCallback cb) {
        callback_ = std::move(cb);
    }

    bool IsListening() const { return listening_; }

private:
    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback callback_;
    bool listening_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
