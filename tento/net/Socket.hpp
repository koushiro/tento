//
// Created by koushiro on 9/2/18.
//

#pragma once

#include <netinet/in.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/Copyable.hpp"
#include "tento/base/NonCopyable.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// Socket Address (only for ipv4 currently)
class SockAddr : Copyable {
public:
    explicit SockAddr(uint16_t port = 0, bool loopback = true);
    SockAddr(const std::string& ip, uint16_t port);
    explicit SockAddr(const std::string& addr); /// addr format: `ip:port`
    ~SockAddr() = default;
    SockAddr(const SockAddr&) = default;
    SockAddr& operator=(const SockAddr&) = default;

    void SetSockAddr(const struct sockaddr_in& addr) { addr_ = addr; }

    const struct sockaddr* GetRaw() const {
        return static_cast<const struct sockaddr*>(
            static_cast<const void*>(&addr_)
        );
    }

    std::string ToIp() const;
    uint16_t ToPort() const;
    std::string ToIpAndPort() const;

private:
    std::vector<std::string> splitIpAndPort(const std::string& address);

private:
    struct sockaddr_in addr_;
};

#define INVALID_SOCKET (-1)

/// Socket file descriptor RAII class, move-only.
class Socket : NonCopyable {
public:
    explicit Socket();
    explicit Socket(int connFd);
    ~Socket();

    Socket(Socket&& rhs) noexcept : sockFd_(rhs.sockFd_) {
        rhs.sockFd_ = INVALID_SOCKET;
    }
    Socket& operator=(Socket&& rhs) noexcept {
        if (this != &rhs) {
            sockFd_ = rhs.sockFd_;
            rhs.sockFd_ = INVALID_SOCKET;
        }
        return *this;
    }

    const int Fd() const { return sockFd_; }

    void Bind(const SockAddr& addr);
    void Listen(int n = SOMAXCONN);
    Socket Accept(SockAddr& peerAddr);

    /// @brief Close socket manually.
    /// You don't have to call this method,
    /// because it will be invoked automatically in the destructor.
    void Close();

//    int Connect(const SockAddr& serverAddr);
//    ssize_t Read(void* buf, size_t count);
//    ssize_t Write(const void* buf, size_t count);

    void ShutdownWrite();
    void SetTcpNoDelay(bool on);
    void SetReuseAddr(bool on);
    void SetReusePort(bool on);
    void SetKeepAlive(bool on);
    void SetTimeout(uint32_t timeout_ms);

private:
    int sockFd_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)