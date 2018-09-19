//
// Created by koushiro on 8/23/18.
//

#include "tento/net/tcp/TcpConnection.hpp"

#include "tento/base/Logger.hpp"
#include "tento/net/Channel.hpp"
#include "tento/net/EventLoop.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

// True if err is an error that means a read/write operation can be retried
#define ERR_RW_RETRIABLE(err) \
    ((err) == EINTR || (err) == EAGAIN)

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
      remoteAddr_(remoteAddr),
      type_(TcpConnType::kIncoming),
      status_(TcpConnStatus::kDisconnected)
{
    channel_->SetReadCallback ([this]() { handleRead();  });
    channel_->SetWriteCallback([this]() { handleWrite(); });
    channel_->SetErrorCallback([this]() { handleError(); });
    channel_->SetCloseCallback([this]() { handleClose(); });
    LOG_TRACE("TcpConnection::TcpConnection, fd = {}, {}",
              socket_->Fd(), AddrToString());
    socket_->SetKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_TRACE("TcpConnection::~TcpConnection, fd = {}, status = {}, type = {}, {}",
              socket_->Fd(), ConnStatusToString(), ConnTypeToString(), AddrToString());
    assert(status_ == TcpConnStatus::kDisconnected);
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

    // if no data in output queue, try writing directly.
    if (!channel_->IsWritable() && sendBuf_.ReadableBytes() == 0) {
        written = send(channel_->Fd(), message, len, MSG_NOSIGNAL);
        if (written == -1) {
            auto errorCode = errno;
            LOG_ERROR("TcpConnection::SendInLoop, send failed, "
                      "an error '{}' was occurred", strerror(errorCode));
            handleError();
            return;
        } else {    // write successfully.
            remain -= written;
            if (remain == 0 && writeCompleteCallback_) {
                loop_->QueueInLoop([this]() { writeCompleteCallback_(shared_from_this()); });
            }
        }
    }

    // if only part of message is sent, put the remaining data into send buffer.
    if (remain > 0) {
        // If the length of the send buffer exceeds the highWaterMark_(user-specified size),
        // a callback is triggered (only once on the rising edge).
        size_t oldMark = sendBuf_.ReadableBytes();
        auto newMark = oldMark + remain;
        if (oldMark < highWaterMark_ && newMark >= highWaterMark_ && highWaterMarkCallback_) {
            loop_->QueueInLoop([this, newMark]() {
                highWaterMarkCallback_(shared_from_this(), newMark);
            });
        }

        // put the remaining data into send buffer
        sendBuf_.Append(static_cast<const char*>(message) + written, remain);

        // start watching the writable event of the channel,
        // send remaining data in handleWrite() when the writable event comes   .
        if (!channel_->IsWritable()) {
            channel_->EnableWriteEvent();
        }
    }
}

void TcpConnection::Close() {
    LOG_TRACE("TcpConnection::Close, fd = {}, status = {}, type = {}, {}",
        socket_->Fd(), ConnStatusToString(), ConnTypeToString(), AddrToString());
    status_ = TcpConnStatus::kDisconnecting;

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
              socket_->Fd(), ConnStatusToString());

    int errorCode = 0;
    ssize_t n = recvBuf_.ReadFromFd(channel_->Fd(), &errorCode);
    if (n < 0) {
        LOG_ERROR("Input buffer ReadFromFd failed, an error '{}' was occurred",
            strerror(errorCode));
        handleError();
    } else if (n > 0) {
        messageCallback_(shared_from_this(), &recvBuf_);
    } else {    // n == 0
        handleClose();
    }
}

void TcpConnection::handleWrite() {
    assert(loop_->IsInLoopThread());
    assert(channel_->IsWritable());
    LOG_TRACE("TcpConnection::handleWrite， fd = {}, status = {}, {}",
              socket_->Fd(), ConnStatusToString(), AddrToString());

    ssize_t n = send(channel_->Fd(),
        sendBuf_.ReadBegin(), sendBuf_.ReadableBytes(), MSG_NOSIGNAL);
    if (n == -1) {
        auto errorCode = errno;
        LOG_ERROR("TcpConnection::handleWrite, send failed, "
                  "an error '{}' was occurred", strerror(errorCode));
        handleError();
    } else {
        // Continue to send data in the send buffer.
        // Once the sending process is complete,
        // stop watching the writable event immediately, avoiding the busy loop.
        sendBuf_.ReadBytes(static_cast<size_t>(n));

        if (sendBuf_.ReadableBytes() == 0) {
            channel_->DisableWriteEvent();
            if (writeCompleteCallback_) {
                loop_->QueueInLoop([this]() { writeCompleteCallback_(shared_from_this()); });
            }
        }
    }
}

void TcpConnection::handleError() {
    LOG_TRACE("TcpConnection::handleError， fd = {}, status = {}",
              socket_->Fd(), ConnStatusToString());
    status_ = TcpConnStatus::kDisconnecting;
    handleClose();
}

void TcpConnection::handleClose() {
    if (IsDisconnected()) return;

    status_ = TcpConnStatus::kDisconnecting;

    assert(loop_->IsInLoopThread());
    LOG_TRACE("TcpConnection::handleClose， fd = {}, status = {}",
              socket_->Fd(), ConnStatusToString());

    channel_->DisableAllEvents();

    TcpConnectionPtr conn(shared_from_this());
    if (connectionCallback_) {
        connectionCallback_(conn);
    }
    if (closeCallback_) {
        closeCallback_(conn);
    }
    status_ = TcpConnStatus::kDisconnected;
}

NAMESPACE_END(net)
NAMESPACE_END(tento)