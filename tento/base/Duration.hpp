//
// Created by koushiro on 8/17/18.
//

#ifndef TENTO_DURATION_HPP
#define TENTO_DURATION_HPP

#include <cstdint>
#include <sys/time.h>

namespace tento {

class Duration {
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
    explicit Duration(double seconds) : ns_(int64_t(seconds * kSecond)) {}

    int64_t Nanoseconds() const { return ns_; }
    double Microseconds() const { return double(ns_) / kMicrosecond; }
    double Milliseconds() const { return double(ns_) / kMillisecond; }
    double Seconds()      const { return double(ns_) / kSecond; }
    double Minutes()      const { return double(ns_) / kMinute; }
    double Hours()        const { return double(ns_) / kHour; }

    struct timeval TimeVal() const {
        struct timeval t;
        t.tv_sec = long(ns_ / kSecond);
        t.tv_usec = long(ns_ % kSecond) / long(kMicrosecond);
        return t;
    }

    bool IsZero() const { return ns_ == 0; }
    bool operator< (const Duration& rhs) const { return ns_ <  rhs.ns_; }
    bool operator<=(const Duration& rhs) const { return ns_ <= rhs.ns_; }
    bool operator> (const Duration& rhs) const { return ns_ >  rhs.ns_; }
    bool operator>=(const Duration& rhs) const { return ns_ >= rhs.ns_; }
    bool operator==(const Duration& rhs) const { return ns_ == rhs.ns_; }

    Duration operator+=(const Duration& rhs) { ns_ += rhs.ns_; return *this; }
    Duration operator-=(const Duration& rhs) { ns_ -= rhs.ns_; return *this; }
    Duration operator*=(int n) { ns_ *= n; return *this; }
    Duration operator/=(int n) { ns_ /= n; return *this; }

private:
    int64_t ns_;    // nanoseconds
};

} // namespace tento

#endif //TENTO_DURATION_HPP
