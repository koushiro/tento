//
// Created by koushiro on 8/17/18.
//

#pragma once

#include <chrono>

#include "tento/base/Copyable.hpp"
#include "tento/base/Duration.hpp"

NAMESPACE_BEGIN(tento)

class Timestamp : public Copyable {
public:
    Timestamp() : ns_(0) {}
    explicit Timestamp(uint64_t nanoseconds) : ns_(nanoseconds) {}

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
        uint64_t ns = ns_ - earlier.ns_;
        return Duration::FromNanos(ns);
    }

    Duration Elapsed() { return DurationSince(Now()); }

private:
    /// ns_ gives the number of nanoseconds elapsed since the Epoch
    /// 1970-01-01 00:00:00 +0000 (UTC).
    uint64_t ns_;
};

NAMESPACE_END(tento)
