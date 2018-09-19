//
// Created by koushiro on 8/17/18.
//

#pragma once

#include <cstdint>
#include <cassert>
#include <ostream>

#include "tento/base/Common.hpp"
#include "tento/base/Copyable.hpp"

NAMESPACE_BEGIN(tento)

class Duration : Copyable {
public:
    /// C++14 number delimiter
    static constexpr uint32_t NANOS_PER_MICRO = 1'000;
    static constexpr uint32_t NANOS_PER_MILLI = 1'000'000;
    static constexpr uint32_t NANOS_PER_SEC = 1'000'000'000;
    static constexpr uint32_t MILLIS_PER_SEC = 1'000;
    static constexpr uint32_t MICROS_PER_SEC = 1'000'000;
    static constexpr uint32_t SECS_PER_MIN = 60;
    static constexpr uint32_t SECS_PER_HOUR = 3600;

public:
    explicit Duration(uint64_t secs, uint32_t nanos)
        : secs_(secs), nanos_(nanos) {}
    ~Duration() = default;
    Duration(const Duration&) = default;
    Duration& operator=(const Duration&) = default;

    static Duration FromNanos(uint64_t nanos) {
        return Duration(
            nanos / NANOS_PER_SEC,
            static_cast<uint32_t>(nanos % NANOS_PER_SEC)
        );
    }

    static Duration FromMicros(uint64_t micros) {
        return Duration(
            micros / MICROS_PER_SEC,
            static_cast<uint32_t>(micros % MICROS_PER_SEC) * NANOS_PER_MICRO
        );
    }

    static Duration FromMillis(uint64_t millis) {
        return Duration(
            millis / MILLIS_PER_SEC,
            static_cast<uint32_t>(millis % MILLIS_PER_SEC) * NANOS_PER_MILLI
        );
    }

    static Duration FromSecs(uint64_t secs) {
        return Duration(secs, 0);
    }

    static Duration FromMins(uint32_t mins) {
        return Duration(mins * SECS_PER_MIN, 0);
    }

    static Duration FromHours(uint32_t hours) {
        return Duration(hours * SECS_PER_HOUR, 0);
    }

    /// Returns the number of whole seconds contained by this Duration.
    ///
    /// The returned value does not include the
    /// fractional (nanosecond/microsecond/millisecond) part of the duration,
    /// which can be obtained using 'SubSecNanos/SubSecMicros/SubSecMillis'.
    uint64_t SecsPart()     const { return secs_; }
    /// Returns the fractional part of this `Duration`, in nanoseconds.
    uint32_t SubSecNanos()  const { return nanos_; }
    /// Returns the fractional part of this Duration, in microseconds.
    uint32_t SubSecMicros() const { return nanos_ / NANOS_PER_MICRO; }
    /// Returns the fractional part of this Duration, in milliseconds.
    uint32_t SubSecMillis() const { return nanos_ / NANOS_PER_MILLI; }

    bool IsZero() const { return secs_ == 0 && nanos_ == 0; }

    /// Maybe overflow
    uint64_t AsNanos() const {
        return secs_ * NANOS_PER_SEC + nanos_;
    }
    double AsMicros() const {
        return secs_ * MICROS_PER_SEC +
            static_cast<double>(nanos_) / NANOS_PER_MICRO;
    }
    double AsMillis() const {
        return secs_ * MILLIS_PER_SEC +
            static_cast<double>(nanos_) / NANOS_PER_MILLI;
    }
    double AsSecs() const {
        return secs_ + static_cast<double>(nanos_) / NANOS_PER_SEC;
    }
    double AsMins() const {
        return static_cast<double>(secs_) / SECS_PER_MIN;
    }
    double AsHours() const {
        return static_cast<double>(secs_) / SECS_PER_HOUR;
    }

    bool operator==(const Duration& rhs) const {
        return secs_ == rhs.secs_ && nanos_ == rhs.nanos_;
    }
    bool operator!=(const Duration& rhs) const {
        return !operator==(rhs);
    }
    bool operator< (const Duration& rhs) const {
        return secs_ < rhs.secs_ && nanos_ <  rhs.nanos_;
    }
    bool operator<=(const Duration& rhs) const {
        return operator<(rhs) && operator==(rhs);
    }
    bool operator> (const Duration& rhs) const {
        return !operator<=(rhs);
    }
    bool operator>=(const Duration& rhs) const {
        return !operator<(rhs);
    }

    Duration& operator+=(const Duration& rhs) {
        nanos_ += rhs.nanos_;
        if (nanos_ >= NANOS_PER_SEC) {
            nanos_ -= NANOS_PER_SEC;
            secs_ += 1;
        }
        secs_ += rhs.secs_;
        return *this;
    }
    Duration& operator-=(const Duration& rhs) {
        assert(rhs.secs_ <= secs_ && rhs.nanos_ <= nanos_);
        if (nanos_ >= rhs.nanos_) {
            nanos_ -= rhs.nanos_;
        } else {
            nanos_ = nanos_ + NANOS_PER_SEC - rhs.nanos_;
            secs_ -= 1;
        }
        secs_ -= rhs.secs_;
        return *this;
    }
    Duration& operator*=(uint32_t n) {
        auto total_nanos = nanos_ * n;
        auto extra_secs = total_nanos / NANOS_PER_SEC;
        nanos_ = total_nanos % NANOS_PER_SEC;
        secs_ = secs_ * n + extra_secs;
        return *this;
    }
    Duration& operator/=(uint32_t n) {
        assert(n != 0);
        auto secs = secs_ / n;
        auto carry = secs_ - secs * n;
        auto extra_nanos = carry * NANOS_PER_SEC / n;
        nanos_ = nanos_ / n + static_cast<uint32_t>(extra_nanos);
        secs_ = secs;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Duration& duration);

private:
    uint64_t secs_;     /// seconds
    uint32_t nanos_;    /// nanoseconds
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

inline std::ostream& operator<<(std::ostream& os, const Duration& duration) {
    os << "Duration { secs: " << duration.secs_
        << ", nanos: " << duration.nanos_ << " }";
    return os;
}

NAMESPACE_END(tento)
