//
// Created by koushiro on 9/2/18.
//

#pragma once

#include <netinet/in.h>
#include <unistd.h>

#include <string>

#include "tento/base/Common.hpp"
#include "tento/base/Copyable.hpp"
#include "tento/base/NonCopyable.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// Wrapper of sockaddr_in
class SockAddr : Copyable {
public:
    explicit SockAddr(uint16_t port = 0, bool loopback = false);
    explicit SockAddr(const struct sockaddr_in addr) : addr_(addr) {}
    SockAddr(const std::string& ip, uint16_t port);
    ~SockAddr() = default;
    SockAddr(const SockAddr&) = default;
    SockAddr& operator=(const SockAddr&) = default;

    void SetSockAddr(const struct sockaddr_in& addr) { addr_ = addr; }

    const struct sockaddr* GetRaw() const {
        return static_cast<const struct sockaddr*>(
            static_cast<const void*>(&addr_)
        );
    }

    struct sockaddr* GetRaw() {
        return static_cast<struct sockaddr*>(
            static_cast<void*>(&addr_)
        );
    }

    sa_family_t GetFamily() const { return addr_.sin_family; }

    std::string ToIp() const;
    uint16_t ToPort() const;
    std::string ToIpAndPort() const;

private:
    struct sockaddr_in  addr_;
};

/// Wrapper of socket file descriptor, thread safe.
class Socket : NonCopyable {
public:
    explicit Socket(int sockFd) : sockFd_(sockFd) {}
    ~Socket() { close(sockFd_); }
    Socket(Socket&& rhs) noexcept : sockFd_(rhs.sockFd_) { rhs.sockFd_ = -1; }
    Socket& operator=(Socket&& rhs) noexcept {
        if (this != &rhs) {
            sockFd_ = rhs.sockFd_;
            rhs.sockFd_ = -1;
        }
        return *this;
    }

    const int Fd() const { return sockFd_; }

    void Bind(const SockAddr& serverAddr);
    void Listen();
    int Accept(SockAddr& clientAddr);

//    int Connect(const SockAddr& serverAddr);
//    ssize_t Read(void* buf, size_t count);
//    ssize_t Write(const void* buf, size_t count);

    void ShutdownWrite();
    void SetTcpNoDelay(bool on);
    void SetReuseAddr(bool on);
    void SetReusePort(bool on);
    void SetKeepAlive(bool on);

private:
    int sockFd_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)