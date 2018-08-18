//
// Created by koushiro on 8/17/18.
//

#ifndef TENTO_TIMESTAMP_HPP
#define TENTO_TIMESTAMP_HPP

#include "Duration.hpp"
#include <chrono>

namespace tento {

class Timestamp {
public:
    Timestamp() : ns_(0) {}
    explicit Timestamp(int64_t nanoseconds) : ns_(nanoseconds) {}
    explicit Timestamp(const struct timeval& t)
        : ns_(int64_t(t.tv_sec) * Duration::kSecond
            + (int64_t)t.tv_usec * Duration::kMicrosecond) {}

    static Timestamp Now() {
        using Nanoseconds = std::chrono::nanoseconds;
        using Clock = std::chrono::system_clock;
        return Timestamp(
            std::chrono::duration_cast<Nanoseconds>(
                Clock::now().time_since_epoch()
            ).count()
        );
    }

    int64_t UnixNanoSec() { return ns_; }
    int64_t UnixMicroSec() { return ns_ / Duration::kMicrosecond; }
    int64_t UnixMilliSec() { return ns_ / Duration::kMillisecond; }
    int64_t UnixSec() { return ns_ / Duration::kSecond; }

    struct timeval TimeVal() const {
        struct timeval t;
        t.tv_sec = (long)(ns_ / Duration::kSecond);
        t.tv_usec = (long)(ns_ % Duration::kSecond) / (long)Duration::kMicrosecond;
        return t;
    }

    bool IsEpoch() const { return ns_ ==  0; }
    bool operator< (const Timestamp& rhs) const { return ns_ <  rhs.ns_; }
    bool operator<=(const Timestamp& rhs) const { return ns_ <= rhs.ns_; }
    bool operator> (const Timestamp& rhs) const { return ns_ > rhs.ns_;  }
    bool operator>=(const Timestamp& rhs) const { return ns_ >= rhs.ns_; }
    bool operator==(const Timestamp& rhs) const { return ns_ == rhs.ns_; }
    bool operator!=(const Timestamp& rhs) const { return ns_ != rhs.ns_; }

    Timestamp& operator+=(const Duration& duration) {
        ns_ += duration.Nanoseconds();
        return *this;
    }

    Timestamp& operator-=(const Duration& duration) {
        ns_ -= duration.Nanoseconds();
        return *this;
    }

    Duration operator- (const Timestamp& rhs) const {
        int64_t ns = ns_ - rhs.ns_;
        return Duration(ns);
    }

private:
    // ns_ gives the number of nanoseconds elapsed since the Epoch
    // 1970-01-01 00:00:00 +0000 (UTC).
    int64_t ns_;
};

inline Timestamp operator+ (const Timestamp& ts, const Duration& duration) {
    Timestamp timestamp = ts;
    timestamp += duration;
    return timestamp;
}

inline Timestamp operator- (const Timestamp& ts, const Duration& duration) {
    Timestamp timestamp = ts;
    timestamp -= duration;
    return timestamp;
}

} // namespace tento

#endif //TENTO_TIMESTAMP_HPP
