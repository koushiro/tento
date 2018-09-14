/// Modified from muduo project https://github.com/chenshuo/muduo
/// See https://github.com/chenshuo/muduo/blob/master/muduo/net/Buffer.h
/// and https://github.com/chenshuo/muduo/blob/master/muduo/net/Buffer.cc

#pragma once

#include <algorithm>
#include <cassert>
#include <cstring>
#include <string>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/Copyable.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

/// A buffer class modeled after org.jboss.netty.buffer.ChannelBuffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0       <=      readeIndex   <=    writeIndex    <=     size
/// |-kCheapPrependSize-|----------- kInitialSize ------------|
/// @endcode
class Buffer : Copyable {
public:
    static constexpr size_t kCheapPrependSize = 8;
    static constexpr size_t kInitialSize = 1024;

public:
    explicit Buffer(size_t initialSize = kInitialSize,
                    size_t prependSize = kCheapPrependSize)
        : buffer_(kCheapPrependSize + initialSize),
          readIndex_(kCheapPrependSize),
          writeIndex_(kCheapPrependSize)
    {
        assert(PrependableBytes() == prependSize);
        assert(ReadableBytes() == 0);
        assert(WritableBytes() == initialSize);
    }
    ~Buffer() = default;
    Buffer(const Buffer&) = default;
    Buffer& operator=(const Buffer&) = default;

public:
    size_t PrependableBytes() const { return readIndex_; }
    size_t ReadableBytes()    const { return writeIndex_ - readIndex_; }
    size_t WritableBytes()    const { return buffer_.size() - writeIndex_; }

    const char* ReadBegin()  const { return buffer_.data() + readIndex_; }
          char* ReadBegin()        { return buffer_.data() + readIndex_; }
    const char* WriteBegin() const { return buffer_.data() + writeIndex_; }
          char* WriteBegin()       { return buffer_.data() + writeIndex_; }

    void ShrinkToFit()    { buffer_.shrink_to_fit(); }
    void Capacity() const { buffer_.capacity(); }
    void Size()     const { buffer_.size(); }

    void Swap(Buffer &rhs) {
        buffer_.swap(rhs.buffer_);
        std::swap(readIndex_, rhs.readIndex_);
        std::swap(writeIndex_, rhs.writeIndex_);
    }

    /// Write
public:
    void Write(const void* data, size_t len) {
        EnsureWritableBytes(len);
        std::memcpy(WriteBegin(), data, len);
        WriteBytes(len);
    }

    void EnsureWritableBytes(size_t len) {
        if (WritableBytes() < len) {
            grow(len);
        }
        assert(WritableBytes() >= len);
    }

    void WriteBytes(size_t n)   { assert(n <= WritableBytes()); writeIndex_ += n; }
    void UnWriteBytes(size_t n) { assert(n <= ReadableBytes()); writeIndex_ -= n; }

    /// Append int64_t/int32_t/int16_t/int8_t with network endian
    void Append(const void* data, size_t len) { Write(data, len); }
    void AppendInt64(int64_t x) { int64_t be = htobe64(x); Append(&be, sizeof(be)); }
    void AppendInt32(int32_t x) { int32_t be = htobe32(x); Append(&be, sizeof(be)); }
    void AppendInt16(int16_t x) { int16_t be = htobe16(x); Append(&be, sizeof(be)); }
    void AppendInt8(int8_t  x)  { Append(&x, sizeof(x)); }

    /// Insert content, specified by the parameter, into the front of readIndex
    void Prepend(const void* data, size_t len) {
        assert(len <= PrependableBytes());
        readIndex_ -= len;
        std::memcpy(ReadBegin(), data, len);
    }
    /// Prepend int64_t/int32_t/int16_t/int8_t with network endian
    void PrependInt64(int64_t x) { int64_t be = htobe64(x); Prepend(&be, sizeof(be)); }
    void PrependInt32(int32_t x) { int32_t be = htobe32(x); Prepend(&be, sizeof(be)); }
    void PrependInt16(int16_t x) { int16_t be = htobe16(x); Prepend(&be, sizeof(be)); }
    void PrependInt8(int8_t  x)  { Prepend(&x, sizeof(x)); }

    /// Peek (Get the first readable byte)
public:
    void Peek(void* data, size_t len) const {
        assert(len <= ReadableBytes());
        std::memcpy(data, ReadBegin(), len);
    }

    /// Peek int64_t/int32_t/int16_t/int8_t with network endian
    int64_t PeekInt64() const { int64_t be = 0; Peek(&be, sizeof(be)); return be64toh(be); }
    int32_t PeekInt32() const { int32_t be = 0; Peek(&be, sizeof(be)); return be32toh(be); }
    int16_t PeekInt16() const { int16_t be = 0; Peek(&be, sizeof(be)); return be16toh(be);  }
    int8_t  PeekInt8()  const { int8_t  x = 0;  Peek(&x, sizeof(x));   return x; }

    /// Read
public:
    void ReadBytes(size_t len) {
        assert(len <= ReadableBytes());
        if (len < ReadableBytes()) {
            readIndex_ += len;
        } else {
            ReadAllBytes();
        }
    }

    void ReadAllBytes() {
        /// Read all readable bytes,
        /// reset readIndex and writeIndex to kCheapPrependSize.
        readIndex_ = kCheapPrependSize;
        writeIndex_ = kCheapPrependSize;
    }

//    std::string ReadBytesAsString(size_t len) {
//        assert(len <= ReadableBytes());
//        std::string res(ReadBegin(), len);
//        ReadBytes(len);
//        return res;
//    }

//    std::string ReadAllBytesAsString() {
//        return ReadBytesAsString(ReadableBytes());
//    }

    /// Read int64_t/int32_t/int16_t/int8_t with network endian
    int64_t ReadInt64() { int64_t p = PeekInt64(); ReadBytes(sizeof(p)); return p; }
    int64_t ReadInt32() { int32_t p = PeekInt32(); ReadBytes(sizeof(p)); return p; }
    int16_t ReadInt16() { int16_t p = PeekInt16(); ReadBytes(sizeof(p)); return p; }
    int8_t  ReadInt8()  { int8_t  p = PeekInt8();  ReadBytes(sizeof(p)); return p; }

    /// Helpers
public:
    const char* FindCRLF(const char* start) const {
        assert(start >= ReadBegin());
        assert(start <= WriteBegin());
        const char* crlf = std::search(start, WriteBegin(), kCRLF, kCRLF + 2);
        return crlf == WriteBegin() ? nullptr : crlf;
    }

    const char* FindCRLF() const {
        return FindCRLF(ReadBegin());
    }

    const char* FindEOL(const char* start) const {
        assert(start >= ReadBegin());
        assert(start <= WriteBegin());
        const void* eol = memchr(start, '\n', WriteBegin() - start);
        return static_cast<const char*>(eol);
    }

    const char* FindEOL() const {
        return FindEOL(ReadBegin());
    }

    ssize_t ReadFromFd(int fd, int* errorCode);

private:
    const char* begin() const { return buffer_.data(); }
          char* begin()       { return buffer_.data(); }

    void grow(size_t len) {
        /// Linux 多线程服务端编程 -- Page 166.
        /// 内部腾挪：经过若干次读写，readIndex移到较后的位置，留下了较大的 prependable 空间。
        /// 若此时想写入的字节数大于 writable 空间，先将已有 readable 数据移动到前面，腾出更多 writable 空间。
        /// 若腾挪之后的 writable 空间仍然不够写入所需的字节数，则要重新分配 buffer 内存（即 resize buffer）。
        if (WritableBytes() + PrependableBytes() < len + kCheapPrependSize) {
            buffer_.resize(writeIndex_ + len);
        } else {
            /// move readable data to the front, make space inside buffer
            assert(kCheapPrependSize < readIndex_);
            size_t readable = ReadableBytes();
            memmove(begin() + kCheapPrependSize, begin() + readIndex_, readable);
            readIndex_ = kCheapPrependSize;
            writeIndex_ = readIndex_ + readable;
            assert(readable == ReadableBytes());
        }
    }

private:
    static constexpr char kCRLF[] = "\r\n";

    std::vector<char> buffer_;
    size_t readIndex_;
    size_t writeIndex_;
};

NAMESPACE_END(net)
NAMESPACE_END(tento)
