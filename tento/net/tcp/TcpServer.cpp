//
// Created by koushiro on 8/23/18.
//

#include "tento/net/tcp/TcpServer.hpp"

#include "tento/base/Logger.hpp"
#include "tento/net/Listener.hpp"
#include "tento/net/EventLoopThreadPool.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

void DefaultConnectionCallback(const TcpConnectionPtr& conn) {
//    LOG_TRACE("{}", conn->AddrToString());
}

void DefaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buffer) {
    buffer->ReadAllBytes();
}

TcpServer::TcpServer(EventLoop* loop, const SockAddr& listenAddr,
                     const std::string& name, uint32_t numThread)
    : loop_(loop),
      listenAddr_(listenAddr),
      name_(name),
      listener_(std::make_unique<Listener>(loop_, listenAddr_)),
      numThread_(numThread),
      connCallback_(DefaultConnectionCallback),
      msgCallback_(DefaultMessageCallback),
      nextConnId_(0)
{
    LOG_TRACE("TcpServer::TcpServer, "
              "name = {}, listening address = {}, num of thread = {}",
              name_, listenAddr_.ToIpAndPort(), numThread_);
    listener_->Listen();
    assert(listener_->IsListening());
}

TcpServer::~TcpServer() {
    LOG_TRACE("TcpServer::~TcpServer", "");
    Stop();
}

void TcpServer::Start() {
    LOG_TRACE("TcpServer::Start", "");
    assert(status_ == Status::kNull);
    status_ = Status::kStarting;

    pool_ = std::make_unique<EventLoopThreadPool>(loop_, numThread_);
    assert(pool_->IsRunning());

    listener_->SetNewConnectionCallback(
        [this](Socket connSock, const SockAddr& remoteAddr) {
            handleNewConnection(std::move(connSock), remoteAddr);
        }
    );
    status_ = Status::kRunning;
    /// NewConnectionCallback will be called in listener channel
    listener_->Accept();
}

void TcpServer::Stop() {
    LOG_TRACE("TcpServer::Stop", "");
    assert(IsRunning());
    status_ = Status::kStopping;
    loop_->RunInLoop([this]() {
        assert(loop_->IsInLoopThread());
        listener_->Stop();
        listener_.reset();

        if (connections_.empty()) {
            LOG_TRACE("no connections", "");
            status_ = Status::kStopped;
            return;
        }

        LOG_TRACE("close connections", "");
        for (auto& pair : connections_) {
            auto id = pair.first;
            auto conn = pair.second;
            if (conn->IsConnected()) {
                LOG_TRACE("close connection id = {}", id);
                conn->Close();
            } else {
                LOG_TRACE("Don't need to call Close for this Connection, "
                          "it may be doing disconnecting. "
                          "Connection id = {}, status = {}",
                          id, conn->ConnStatusToString());
            }
        }
    });
}

void TcpServer::handleNewConnection(Socket&& connSock, const SockAddr& remoteAddr) {
    LOG_TRACE("New connection socket fd = {}", connSock.Fd());
    assert(loop_->IsInLoopThread());
    assert(IsRunning());

    EventLoop* ioLoop = pool_->GetNextLoop();
    ++nextConnId_;  /// From 1 to ...
    std::string name = name_ + "-" + remoteAddr.ToIpAndPort() +
        "#" + std::to_string(nextConnId_);
    TcpConnectionPtr conn = std::make_shared<TcpConnection>(
        ioLoop, nextConnId_, name, std::move(connSock), listenAddr_, remoteAddr
    );

    /// conn set callbacks
    conn->SetConnectionCallback(connCallback_);
    conn->SetMessageCallback(msgCallback_);
    conn->SetCloseCallback([this](const TcpConnectionPtr& conn) {
        auto callback = [this, conn]() {
            // Remove the connection in the listening EventLoop.
            assert(loop_->IsInLoopThread());
            connections_.erase(conn->Id());
            if (IsStopping() && connections_.empty()) {
                status_ = Status::kStopped;
            }
        };
        loop_->RunInLoop(callback);
    });
//    ioLoop->RunInLoop(std::bind(&TcpConnection::OnAttachedToLoop, conn));
    connections_[conn->Id()] = conn;
}

NAMESPACE_END(net)
NAMESPACE_END(tento)