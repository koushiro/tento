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
class TcpConnection : NonCopyable,
                      public std::enable_shared_from_this<TcpConnection> {
public:
    enum class TcpConnType {
        kIncoming,  /// The type of a TcpConnection held by a TcpServer
        kOutgoing,  /// The type of a TcpConnection held by a TcpClient
    };

    enum class TcpConnStatus {
        kConnecting,
        kConnected,
        kDisconnecting,
        kDisconnected,
    };

public:
    TcpConnection(EventLoop* ioLoop,
                  uint64_t id,
                  const std::string& name,
                  Socket&& connSock,            /// socket that was created by client or server.
                  const SockAddr& localAddr,
                  const SockAddr& remoteAddr);
    ~TcpConnection();

    void Send(const void* message, size_t len);
    void Send(const std::string& message);
    void Send(Buffer& message);

    void Close();

public:
    /// These methods are visible only for TcpServer and TcpClient.
    void SetConnType(TcpConnType type) {
        type_ = type;
    }
    void SetConnectionCallback(const ConnectionCallback& cb) {
        connectionCallback_ = cb;
    }
    void SetMessageCallback(const MessageCallback& cb) {
        messageCallback_ = cb;
    }
    void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
        writeCompleteCallback_ = cb;
    }
    void SetHighWaterMartCallback(const HighWaterMarkCallback& cb, size_t highWaterMark) {
        highWaterMarkCallback_ = cb;
        highWaterMark_ = highWaterMark;
    }
    void SetCloseCallback(const CloseCallback& cb) {
        closeCallback_ = cb;
    }

public:
    EventLoop* Loop()       const { return loop_; }
    uint64_t Id()           const { return id_; }
    std::string Name()      const { return name_; }
    bool IsIncomingConn()   const { return type_ == TcpConnType::kIncoming; }
    bool IsOutgoingConn()   const { return type_ == TcpConnType::kOutgoing; }
    bool IsConnecting()     const { return status_ == TcpConnStatus::kConnecting; }
    bool IsConnected()      const { return status_ == TcpConnStatus::kConnected; }
    bool IsDisconnecting()  const { return status_ == TcpConnStatus::kDisconnecting; }
    bool IsDisconnected()   const { return status_ == TcpConnStatus::kDisconnected; }

    std::string ConnTypeToString() const {
        switch (type_) {
            case TcpConnType::kIncoming: return "kIncoming";
            case TcpConnType::kOutgoing: return "kOutgoing";
        }
    }

    std::string ConnStatusToString() const {
        switch (status_.load()) {
            case TcpConnStatus::kConnecting:   return "kConnecting";
            case TcpConnStatus::kConnected:    return "kConnected";
            case TcpConnStatus::kDisconnecting:return "kDisconnecting";
            case TcpConnStatus::kDisconnected: return "kDisconnected";
        }
    }

    std::string AddrToString() const {
        return IsIncomingConn()
            ? "[remote: " + remoteAddr_.ToIpAndPort() + "]->[local: " + localAddr_.ToIpAndPort() + "]"
            : "[local: " + localAddr_.ToIpAndPort() + "]->[remote: " + remoteAddr_.ToIpAndPort() + "]";
    }

private:
    void handleRead();
    void handleWrite();
    void handleError();
    void handleClose();
    void sendInLoop(const void* message, size_t len);

private:
    EventLoop* loop_;
    uint64_t id_;
    std::string name_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    SockAddr localAddr_;
    SockAddr remoteAddr_;

    Buffer recvBuf_;
    Buffer sendBuf_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    size_t highWaterMark_ = 128 * 1024 * 1024; // Default 128MB
    CloseCallback closeCallback_;

    TcpConnType type_;
    std::atomic<TcpConnStatus> status_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
