//
// Created by koushiro on 8/23/18.
//

#pragma once

#include <atomic>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"
#include "tento/net/Alias.hpp"
#include "tento/net/ServerStatus.hpp"
#include "tento/net/Socket.hpp"
#include "tento/net/tcp/TcpConnection.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class EventLoop;
class Listener;
class EventLoopThreadPool;

/// We can use this class to create a TCP server.
/// The typical usage is :
///      1. Create a TcpServer object
///      2. Set the connection callback and message callback
///      3. Call Start()
///      4. Process TCP client connections and messages in callbacks
///      5. At last call Server::Stop() to stop the whole server.
class TcpServer : NonCopyable, public ServerStatus {
public:
    /// when numThread == 0, it's a single thread tcp server.
    TcpServer(EventLoop* loop, const SockAddr& listenAddr,
              const std::string& name, uint32_t numThread);
    ~TcpServer();

    /// Set connection callback.
    /// Not thread safe.
    void SetConnectionCallback(ConnectionCallback cb) {
        connCallback_ = std::move(cb);
    }

    /// Set message callback.
    /// Not thread safe.
    void SetMessageCallback(MessageCallback cb) {
        msgCallback_ = std::move(cb);
    }

    /// @brief Starts the server if it's not listening.
    /// It's harmless to call it multiple times.
    /// Thread safe.
    void Start();

    /// @brief Stop the TCP server manually.
    /// If you forget to call this method,
    /// it will be invoked automatically in the destructor.
    void Stop();

private:
    void handleNewConnection(Socket&& connSock, const SockAddr& remoteAddr);

private:
    using ConnectionMap = std::map<uint64_t /* the id of the connection */, TcpConnectionPtr>;

    EventLoop* loop_;    /// the listening event loop.
    SockAddr listenAddr_;
    const std::string name_;
    std::unique_ptr<Listener> listener_;

    uint32_t numThread_;
    std::unique_ptr<EventLoopThreadPool> pool_;

    ConnectionCallback connCallback_;
    MessageCallback msgCallback_;

    // always in the listening loop thread.
    uint64_t nextConnId_;
    ConnectionMap connections_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)