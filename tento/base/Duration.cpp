//
// Created by koushiro on 8/17/18.
//

#include "Duration.hpp"

namespace tento {
const int64_t Duration::kNanosecond = 1LL;
const int64_t Duration::kMicrosecond = 1000;
const int64_t Duration::kMillisecond = 1000 * kMicrosecond;
const int64_t Duration::kSecond = 1000 * kMillisecond;
const int64_t Duration::kMinute = 60 * kSecond;
const int64_t Duration::kHour = 60 * kMinute;
}