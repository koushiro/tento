//
// Created by koushiro on 8/17/18.
//

#pragma once

#include <chrono>

#include "tento/base/Copyable.hpp"
#include "tento/base/Duration.hpp"

NAMESPACE_BEGIN(tento)

class Timestamp : Copyable {
public:
    Timestamp() : nanos_(0) {}
    explicit Timestamp(uint64_t nanos) : nanos_(nanos) {}
    ~Timestamp() = default;
    Timestamp(const Timestamp&) = default;
    Timestamp& operator=(const Timestamp&) = default;

    static Timestamp Invalid() { return Timestamp(); }

    bool IsValid() const { return nanos_ > 0; }

    static Timestamp Now() {
        using Nanoseconds = std::chrono::nanoseconds;
        using Clock = std::chrono::high_resolution_clock;
        return Timestamp(
            static_cast<uint64_t>(
                std::chrono::duration_cast<Nanoseconds>(
                    Clock::now().time_since_epoch()
                ).count()
            )
        );
    }

    Duration DurationSince (const Timestamp& earlier) const {
        assert(nanos_ > earlier.nanos_);
        uint64_t ns = nanos_ - earlier.nanos_;
        return Duration::FromNanos(ns);
    }

    Duration Elapsed() { return Now().DurationSince(*this); }

    bool operator==(const Timestamp& rhs) const {
        return nanos_ == rhs.nanos_;
    }
    bool operator!=(const Timestamp& rhs) const {
        return !operator==(rhs);
    }
    bool operator< (const Timestamp& rhs) const {
        return nanos_ <  rhs.nanos_;
    }
    bool operator<=(const Timestamp& rhs) const {
        return operator<(rhs) && operator==(rhs);
    }
    bool operator> (const Timestamp& rhs) const {
        return !operator<=(rhs);
    }
    bool operator>=(const Timestamp& rhs) const {
        return !operator<(rhs);
    }

    // Maybe overflow
    Timestamp& operator+=(const Duration& dur) {
        auto nanos = dur.SecsPart() * Duration::NANOS_PER_SEC + dur.SubSecNanos();
        nanos_ += nanos;
        return *this;
    }

    Timestamp& operator-=(const Duration& dur) {
        auto nanos = dur.SecsPart() * Duration::NANOS_PER_SEC + dur.SubSecNanos();
        assert(nanos_ > nanos);
        nanos_ -= nanos;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp);

private:
    /// ns_ gives the number of nanoseconds elapsed since the Epoch
    /// 1970-01-01 00:00:00 +0000 (UTC).
    uint64_t nanos_;
};

inline Timestamp operator+(const Timestamp& when, const Duration& dur) {
    Timestamp time = when;
    time += dur;
    return time;
}

inline Timestamp operator-(const Timestamp& when, const Duration& dur) {
    Timestamp time = when;
    time -= dur;
    return time;
}

inline std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp) {
    os << "Timestamp { nanos: " << timestamp.nanos_ << " }";
    return os;
}

NAMESPACE_END(tento)
