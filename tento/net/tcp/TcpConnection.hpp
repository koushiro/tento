//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <atomic>
#include <memory>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/Alias.hpp"
#include "tento/net/Buffer.hpp"
#include "tento/net/Socket.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class Channel;
class EventLoop;

/// TCP connection for both client and server usage.
/// This is a interface class.
class TcpConnection : NonCopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
    enum class Type {
        kIncoming,  /// The type of a TcpConnection held by a TcpServer
        kOutgoing,  /// The type of a TcpConnection held by a TcpClient
    };
    enum class Status {
        kDisconnected,
        kConnecting,    /// initial status.
        kConnected,
        kDisconnecting,
    };
public:
    TcpConnection(EventLoop* ioLoop,
                  uint64_t id,
                  const std::string& name,
                  Socket&& connSock,            /// socket that was created by client or server.
                  const SockAddr& localAddr,
                  const SockAddr& remoteAddr);

public:
    void SetConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }

    void SetMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }

    /// For internal usage only.
    void SetCloseCallback(const CloseCallback& cb) {
        closeCallback_ = cb;
    }

    uint64_t Id() const { return id_; }

private:
    void setStatus(Status status) { status_ = status; }

    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();

private:
    EventLoop* loop_;
    uint64_t id_;
    std::string name_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    SockAddr localAddr_;
    SockAddr remoteAddr_;
//    Buffer inputBuf_;
//    Buffer outputBuf_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    CloseCallback closeCallback_;

    Type type_;
    std::atomic<Status> status_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
