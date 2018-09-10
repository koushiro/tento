//
// Created by koushiro on 8/23/18.
//

#include "tento/net/tcp/TcpConnection.hpp"
#include "TcpConnection.hpp"

#include "tento/net/Channel.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

TcpConnection::TcpConnection(EventLoop* ioLoop,
                             uint64_t id,
                             const std::string& name,
                             Socket&& connSock,
                             const SockAddr& localAddr,
                             const SockAddr& remoteAddr)
    : loop_(ioLoop),
      id_(id),
      name_(name),
      socket_(std::make_unique<Socket>(std::move(connSock))),
      channel_(std::make_unique<Channel>(loop_, socket_->Fd())),
      localAddr_(localAddr),
      remoteAddr_(remoteAddr)
{

}

NAMESPACE_END(net)
NAMESPACE_END(tento)