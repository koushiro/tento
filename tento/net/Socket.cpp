//
// Created by koushiro on 9/2/18.
//

#include "tento/net/Socket.hpp"

#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <cstring>
#include <cassert>

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

SockAddr::SockAddr(uint16_t port, bool loopback) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    in_addr_t ip = loopback ? INADDR_LOOPBACK : INADDR_ANY;
    addr_.sin_addr.s_addr = htonl(ip);
    addr_.sin_port = htons(port);
}

SockAddr::SockAddr(const std::string& ip, uint16_t port) {
    memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    int ret = inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr.s_addr);
    assert(ret == 1);
    addr_.sin_port = htons(port);
}

std::string SockAddr::ToIp() const {
    char buf[INET_ADDRSTRLEN];
    const char* ret = inet_ntop(AF_INET, &addr_.sin_addr.s_addr, buf, sizeof(buf));
    assert(ret != nullptr);
    return std::string(buf);
}

uint16_t SockAddr::ToPort() const {
    return ntohs(addr_.sin_port);
}

std::string SockAddr::ToIpAndPort() const {
    std::string ret = ToIp();
    ret.push_back(':');
    return ret.append(std::to_string(ToPort()));
}

////////////////////////////////////////////////////////////////////////////////

void Socket::Bind(const SockAddr& serverAddr) {
    socklen_t addrLen = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    int ret = bind(sockFd_, serverAddr.GetRaw(), addrLen);
    assert(ret == 0);
}

void Socket::Listen() {
    int ret = listen(sockFd_, SOMAXCONN);
    assert(ret == 0);
}

int Socket::Accept(SockAddr& clientAddr) {
    socklen_t addrLen = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    int connFd = accept4(sockFd_, clientAddr.GetRaw(), &addrLen,
                         SOCK_NONBLOCK | SOCK_CLOEXEC); /// See man accept4
    assert(connFd != -1);
    return connFd;
}

void Socket::ShutdownWrite() {
    int ret = shutdown(sockFd_, SHUT_WR);
    assert(ret == 0);
}

void Socket::SetTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, IPPROTO_TCP, TCP_NODELAY,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    assert(ret == 0);
}

void Socket::SetReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    assert(ret == 0);
}

void Socket::SetReusePort(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    assert(ret == 0);
}

void Socket::SetKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, SOL_SOCKET, SO_KEEPALIVE,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    assert(ret == 0);
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
