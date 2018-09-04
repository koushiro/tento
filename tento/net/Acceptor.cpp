//
// Created by koushiro on 8/23/18.
//

#include "tento/net/Acceptor.hpp"

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

int CreateSocket(sa_family_t family) {
    int sockFd = socket(family,
                        SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                        IPPROTO_TCP);
    if (sockFd == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("CreateSocket - socket() failed, "
                     "an error '{}' occurred", strerror(errorCode));
        abort();
    }
    return sockFd;
}

Acceptor::Acceptor(EventLoop* loop, const SockAddr& listenAddr)
    : loop_(loop),
      acceptSocket_(CreateSocket(listenAddr.GetFamily())),
      acceptChannel_(loop, acceptSocket_.Fd()),
      callback_(nullptr),
      listening_(false)
{
    acceptSocket_.Bind(listenAddr);
    acceptSocket_.SetReuseAddr(true);
    acceptSocket_.SetReusePort(true);
    acceptChannel_.SetReadCallback([&]() {
        /// handle read event
        loop_->AssertInLoopThread();
        SockAddr peerAddr;
        int connFd = acceptSocket_.Accept(peerAddr);
        if (connFd != -1) {
            Socket sock(connFd);
            if (callback_) {
                callback_(std::move(sock), peerAddr);
            }
        }
    });
}

Acceptor::~Acceptor() {
    acceptChannel_.DisableAllEvents();
    acceptChannel_.Remove();
}

void Acceptor::Listen() {
    loop_->AssertInLoopThread();
    listening_ = true;
    acceptSocket_.Listen();
    acceptChannel_.EnableReadEvent();
}

NAMESPACE_END(net)
NAMESPACE_END(tento)