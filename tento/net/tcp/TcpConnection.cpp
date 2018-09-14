//
// Created by koushiro on 8/23/18.
//

#include "tento/net/tcp/TcpConnection.hpp"

#include "tento/base/Logger.hpp"
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
    LOG_TRACE("TcpConnection::TcpConnection, addr = {}", AddrToString());
    channel_->SetReadCallback ([this]() { handleRead();  });
    channel_->SetWriteCallback([this]() { handleWrite(); });
    channel_->SetCloseCallback([this]() { handleClose(); });
    channel_->SetErrorCallback([this]() { handleError(); });
    socket_->SetKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_TRACE("TcpConnection::~TcpConnection, status = {}, addr = {}",
        StatusToString(), AddrToString());
    assert(status_ == Status::kDisconnected);
}

void TcpConnection::Send(const void* message, size_t len) {
    if (!IsConnected()) return;

    if (loop_->IsInLoopThread()) {
        sendInLoop(message, len);
    } else {
        loop_->RunInLoop([this, message, len]() {
            this->shared_from_this()->sendInLoop(message, len);
        });
    }
}

void TcpConnection::Send(const std::string& message) {
    Send(message.data(), message.size());
}

void TcpConnection::Send(Buffer& message) {
    if (!IsConnected()) return;

    if (loop_->IsInLoopThread()) {
        sendInLoop(message.ReadBegin(), message.ReadableBytes());
        message.ReadAllBytes();
    } else {
        loop_->RunInLoop([this, &message]() {
            this->shared_from_this()
                ->sendInLoop(message.ReadBegin(), message.ReadableBytes());
        });
        message.ReadAllBytes();
    }
}

void TcpConnection::sendInLoop(const void* message, size_t len) {
    assert(loop_->IsInLoopThread());

    if (IsDisconnected()) {
        LOG_TRACE("connection disconnected, give up writing", "");
    }

    ssize_t written = 0;
    size_t remain = len;

    // if no data in output queue, writing directly.
    if (!channel_->IsWritable() && outputBuf_.ReadableBytes() == 0) {
        written = send(channel_->Fd(), message, len, MSG_NOSIGNAL);
        if (written == -1) {
            auto errorCode = errno;
            LOG_ERROR("TcpConnection::SendInLoop, send failed, "
                      "an error '{}' was occurred", strerror(errorCode));
            handleError();
            return;
//            if (errorCode == EPIPE || errorCode == ECONNRESET) {
//                handleError();
//                return;
//            }
        } else {    // write successfully.
            remain -= written;
            if (remain == 0 && writeCompleteCallback_) {
                loop_->QueueInLoop([this]() {
                    writeCompleteCallback_(shared_from_this());
                });
            }
        }
    }

    if (remain > 0) {
        size_t oldLen = outputBuf_.ReadableBytes();
        auto mark = oldLen + remain;
        if (mark >= highWaterMark_ && oldLen < highWaterMark_ && highWaterMarkCallback_) {
            loop_->QueueInLoop([this, mark]() {
                highWaterMarkCallback_(shared_from_this(), mark);
            });
        }

        outputBuf_.Append(static_cast<const char*>(message) + written, remain);
        if (!channel_->IsWritable()) {
            channel_->EnableWriteEvent();
        }
    }
}

void TcpConnection::Close() {
    LOG_TRACE("TcpConnection::Close, fd = {}, status = {}, addr = ",
        socket_->Fd(), StatusToString(), AddrToString());
    status_ = Status::kDisconnecting;

    auto conn = shared_from_this();
    loop_->QueueInLoop([conn]() {
        assert(conn->loop_->IsInLoopThread());
        conn->handleClose();
    });
}

///////////////////////////////////////////////////////////////////////////////

void TcpConnection::handleRead() {
    assert(loop_->IsInLoopThread());
    LOG_TRACE("TcpConnection::handleWrite， fd = {}, status = {}",
              socket_->Fd(), StatusToString());
    int errorCode = 0;
    ssize_t n = inputBuf_.ReadFromFd(channel_->Fd(), &errorCode);
    if (n < 0) {
        LOG_ERROR("Input buffer ReadFromFd failed, an error '{}' was occurred",
            strerror(errorCode));
        handleError();
    } else if (n > 0) {
        messageCallback_(shared_from_this(), &inputBuf_);
    } else {    // n == 0
        handleClose();
    }
}

void TcpConnection::handleWrite() {
    assert(loop_->IsInLoopThread());
    assert(channel_->IsWritable());
    LOG_TRACE("TcpConnection::handleWrite， fd = {}, status = {}",
              socket_->Fd(), StatusToString());
    ssize_t n = send(channel_->Fd(), outputBuf_.ReadBegin(), outputBuf_.ReadableBytes(), MSG_NOSIGNAL);
    if (n == -1) {
        auto errorCode = errno;
        LOG_ERROR("TcpConnection::handleWrite, send failed, "
                  "an error '{}' was occurred", strerror(errorCode));
        handleError();
    } else {
        outputBuf_.ReadBytes(n);
        if (outputBuf_.ReadableBytes() == 0) {
            channel_->DisableWriteEvent();
            if (writeCompleteCallback_) {
                loop_->QueueInLoop([this]() {
                    writeCompleteCallback_(shared_from_this());
                });
            }
        }
    }
}

void TcpConnection::handleClose() {
    if (IsDisconnected()) return;

    assert(loop_->IsInLoopThread());
    status_ = Status::kDisconnecting;
    LOG_TRACE("TcpConnection::handleClose， fd = {}, status = {}",
              socket_->Fd(), StatusToString());
    channel_->DisableAllEvents();

    TcpConnectionPtr conn(shared_from_this());
    if (connectionCallback_) {
        connectionCallback_(conn);
    }
    if (closeCallback_) {
        closeCallback_(conn);
    }
    status_ = Status::kDisconnected;
}

void TcpConnection::handleError() {
    LOG_TRACE("TcpConnection::handleError， fd = {}, status = {}",
        socket_->Fd(), StatusToString());
    status_ = Status::kDisconnecting;
    handleClose();
}

NAMESPACE_END(net)
NAMESPACE_END(tento)