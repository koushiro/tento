//
// Created by koushiro on 8/17/18.
//

#include "tento/net/Buffer.hpp"

#include <cerrno>
#include <sys/uio.h>

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

ssize_t net::Buffer::ReadFromFd(int fd, int* errorCode) {
    // saved an ioctl()/FIONREAD call to tell how much to read
    char extrabuf[65536];
    struct iovec vec[2];
    const size_t writable = WritableBytes();
    vec[0].iov_base = begin() + writeIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    // when there is enough space in this buffer, don't read into extrabuf.
    // when extrabuf is used, we read 64k bytes at most.
    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    const ssize_t n = readv(fd, vec, iovcnt);

    if (n < 0) {
        *errorCode = errno;
    } else if (static_cast<size_t>(n) <= writable) {
        writeIndex_ += n;
    } else {
        writeIndex_ = buffer_.size();
        Append(extrabuf, n - writable);
    }

    return n;
}

NAMESPACE_END(net)
NAMESPACE_END(tento)
