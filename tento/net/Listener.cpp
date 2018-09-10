//
// Created by koushiro on 8/23/18.
//

#include "tento/net/Listener.hpp"

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

Listener::Listener(EventLoop* loop, const SockAddr& listenAddr)
    : loop_(loop),
      listenSocket_(Socket()),
      listenAddr_(listenAddr),
      channel_(std::make_unique<Channel>(loop, listenSocket_.Fd())),
      newConnCallback_(nullptr),
      listening_(false)
{
    LOG_TRACE("Listener::Listener, listenAddr = {}", listenAddr_.ToIpAndPort());
    listenSocket_.Bind(listenAddr_);

    listenSocket_.SetReuseAddr(true);
    listenSocket_.SetReusePort(true);
}

Listener::~Listener() {
    LOG_TRACE("Listener::~Listener", "");
    Stop();
}

void Listener::Listen(int n) {
    LOG_TRACE("Listener::Listen, {} connection requests will be queued", n);
    loop_->AssertInLoopThread();
    listening_ = true;
    listenSocket_.Listen(n);
}

void Listener::Accept() {
    LOG_TRACE("Listener::Accept", "");

    channel_->SetReadCallback([this]() {
        /// handle read event
        loop_->AssertInLoopThread();
        SockAddr remoteAddr;
        Socket connSock = listenSocket_.Accept(remoteAddr);
        if (connSock.Fd() != -1) {
            LOG_TRACE("Listener::Accept, Accepted a new connection from {}",
                      remoteAddr.ToIpAndPort());
            connSock.SetKeepAlive(true);
            if (newConnCallback_) {
                newConnCallback_(std::move(connSock), remoteAddr);
            }
        }
    });

    channel_->EnableReadEvent();

    LOG_TRACE("Server is running at {}", listenAddr_.ToIpAndPort());
}

void Listener::Stop() {
    channel_->DisableAllEvents();
    channel_->Remove();
}

NAMESPACE_END(net)
NAMESPACE_END(tento)