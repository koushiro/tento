//
// Created by koushiro on 8/17/18.
//

#include "Duration.hpp"

namespace tento {
static const int64_t kNanosecond = 1LL;
static const int64_t kMicrosecond = 1000;
static const int64_t kMillisecond = 1000 * kMicrosecond;
static const int64_t kSecond = 1000 * kMillisecond;
static const int64_t kMinute = 60 * kSecond;
static const int64_t kHour = 60 * kMinute;
}