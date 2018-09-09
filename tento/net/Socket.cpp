//
// Created by koushiro on 9/2/18.
//

#include "tento/net/Socket.hpp"

#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <cstring>
#include <cassert>

#include "tento/base/Logger.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

bool SplitIpAndPort(const std::string& address, std::string& ip, uint16_t& port) {
    /// TODO
    return false;
}

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

SockAddr::SockAddr(const std::string& addr) {
    std::string ip;
    uint16_t port;
    {
        bool ret = SplitIpAndPort(addr, ip, port);
        assert(ret);
    }
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

int CreateSocket(sa_family_t family = AF_INET) {
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

Socket::Socket()
    : sockFd_(CreateSocket())
{
    LOG_TRACE("Socket::Socket, create socket, fd = {}", sockFd_);
}

Socket::Socket(int connFd)
    : sockFd_(connFd)
{
    if (sockFd_ == kInvalidSocket) {
        LOG_ERROR("Socket::Socket(int connFd), connFd is invalid socket fd");
    } else {
        LOG_TRACE("Socket::Socket, create socket, fd = {}", sockFd_);
    }
}

Socket::~Socket() {
    if (sockFd_ == kInvalidSocket) {
        // This object has been closed or moved.
        // So we don't need to call close.
        return;
    }
    Close();
}

void Socket::Bind(const SockAddr& addr) {
    auto addrLen = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    int ret = bind(sockFd_, addr.GetRaw(), addrLen);
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::Bind - bind() failed, "
                     "an error '{}' occurred", strerror(errorCode));
        abort();
    }
}

void Socket::Listen(int n) {
    int ret = listen(sockFd_, n);
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::Listen - listen() failed, "
                     "an error '{}' occurred", strerror(errorCode));
        abort();
    }
}

Socket Socket::Accept(SockAddr& peerAddr) {
    struct sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(addr_in));
    auto addr = static_cast<struct sockaddr*>(static_cast<void*>(&addr_in));
    auto addrLen = static_cast<socklen_t>(sizeof(struct sockaddr_in));
    int connFd = accept4(sockFd_, addr, &addrLen,
                         SOCK_NONBLOCK | SOCK_CLOEXEC); /// See man accept4
    if (connFd == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::Accept - accept4() failed, "
                     "an error '{}' occurred", strerror(errorCode));
        switch (errorCode) {
            case EAGAIN:        /// The socket is marked nonblocking and no connections are present to be accepted.
            case ECONNABORTED:  ///  A connection has been aborted.
            case EINTR:         /// The system call was interrupted by a signal that was caught before a valid connection arrived.
            case EMFILE:        /// The per-process limit on the number of open file descriptors has been reached
            case EPERM:         /// Firewall rules forbid connection.
                break;
            case EBADF:         /// sockfd is not an open file descriptor.
            case EFAULT:        /// The addr argument is not in a writable part of the user address space.
            case EINVAL:        /// Socket is not listening for connections, or addrlen or flags is invalid.
            case ENFILE:        /// The system-wide limit on the total number of open files has been reached.
            case ENOBUFS:       /// Not  enough  free  memory.
            case ENOMEM:        /// Not  enough  free  memory.
            case ENOTSOCK:      /// sockfd does not refer to a socket.
            case EOPNOTSUPP:    /// The referenced socket is not of type SOCK_STREAM.
            case EPROTO:        /// Protocol error.
                abort();
            default:            /// Unknown error.
                abort();
        }
    } else {
        peerAddr.SetSockAddr(addr_in);
    }
    return Socket(connFd);
}

void Socket::Close() {
    if (sockFd_ == kInvalidSocket) {
        // This object has been closed or moved.
        // So we don't need to call close.
        return;
    }

    LOG_TRACE("Socket::Close, close socket, fd = {}", sockFd_);
    close(sockFd_);
    sockFd_ = kInvalidSocket;
}

void Socket::ShutdownWrite() {
    int ret = shutdown(sockFd_, SHUT_WR);
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::ShutdownWrite - shutdown() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

void Socket::SetTcpNoDelay(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, IPPROTO_TCP, TCP_NODELAY,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::SetTcpNoDelay - setsockopt() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

void Socket::SetReuseAddr(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::SetReuseAddr - setsockopt() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

void Socket::SetReusePort(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, SOL_SOCKET, SO_REUSEPORT,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::SetReusePort - setsockopt() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

void Socket::SetKeepAlive(bool on) {
    int optval = on ? 1 : 0;
    int ret = setsockopt(sockFd_, SOL_SOCKET, SO_KEEPALIVE,
                         &optval, static_cast<socklen_t>(sizeof(optval)));
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::SetKeepAlive - setsockopt() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

void Socket::SetTimeout(uint32_t timeout_ms) {
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    int ret = setsockopt(sockFd_, SOL_SOCKET, SO_RCVTIMEO,
                         (const char*)&tv, sizeof(tv));
    if (ret == -1) {
        auto errorCode = errno;
        LOG_CRITICAL("Socket::SetTimeout - setsockopt() failed, "
                     "an error '{}' occurred", strerror(errorCode));
    }
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
