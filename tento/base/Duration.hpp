//
// Created by koushiro on 8/17/18.
//

#pragma once

#include <sys/time.h>

#include <cstdint>

#include "tento/base/Common.hpp"
#include "tento/base/Copyable.hpp"

NAMESPACE_BEGIN(tento)

class Duration : public Copyable {
public:
    static const int64_t kNanosecond;   // = 1LL
    static const int64_t kMicrosecond;  // = 1000
    static const int64_t kMillisecond;  // = 1000 * kMicrosecond
    static const int64_t kSecond;       // = 1000 * kMillisecond
    static const int64_t kMinute;       // = 60 * kSecond
    static const int64_t kHour;         // = 60 * kMinute
public:
    Duration() : ns_(0) {}
    explicit Duration(const struct timeval& t)
        : ns_(t.tv_sec * kSecond + t.tv_usec * kMicrosecond) {}
    explicit Duration(int64_t nanoseconds) : ns_(nanoseconds) {}
    explicit Duration(int nanoseconds) : ns_(nanoseconds) {}
    explicit Duration(double seconds)
        : ns_(static_cast<int64_t>(seconds * kSecond)) {}

    int64_t Nanoseconds() const { return ns_; }
    double Microseconds() const { return double(ns_) / kMicrosecond; }
    double Milliseconds() const { return double(ns_) / kMillisecond; }
    double Seconds()      const { return double(ns_) / kSecond; }
    double Minutes()      const { return double(ns_) / kMinute; }
    double Hours()        const { return double(ns_) / kHour; }

    struct timeval TimeVal() const {
        struct timeval t;
        t.tv_sec = static_cast<long>(ns_ / kSecond);
        t.tv_usec = static_cast<long>(ns_ % kSecond) /
                    static_cast<long>(kMicrosecond);
        return t;
    }

    bool IsZero() const { return ns_ == 0; }
    bool operator< (const Duration& rhs) const { return ns_ <  rhs.ns_; }
    bool operator<=(const Duration& rhs) const { return ns_ <= rhs.ns_; }
    bool operator> (const Duration& rhs) const { return ns_ >  rhs.ns_; }
    bool operator>=(const Duration& rhs) const { return ns_ >= rhs.ns_; }
    bool operator==(const Duration& rhs) const { return ns_ == rhs.ns_; }
    bool operator!=(const Duration& rhs) const { return ns_ != rhs.ns_; }

    Duration& operator+=(const Duration& rhs) { ns_ += rhs.ns_; return *this; }
    Duration& operator-=(const Duration& rhs) { ns_ -= rhs.ns_; return *this; }
    Duration& operator*=(int n) { ns_ *= n; return *this; }
    Duration& operator/=(int n) { ns_ /= n; return *this; }

private:
    int64_t ns_;    // nanoseconds
};

inline Duration operator+ (const Duration& lhs, const Duration& rhs) {
    Duration d = lhs;
    d += rhs;
    return d;
}

inline Duration operator- (const Duration& lhs, const Duration& rhs) {
    Duration d = lhs;
    d -= rhs;
    return d;
}

inline Duration operator* (const Duration& lhs, int n) {
    Duration d = lhs;
    d *= n;
    return d;
}

inline Duration operator* (int n, const Duration& rhs) {
    return rhs * n;
}

inline Duration operator/ (const Duration& lhs, int n) {
    Duration d = lhs;
    d /= n;
    return d;
}

NAMESPACE_END(tento)
