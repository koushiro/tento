//
// Created by koushiro on 9/3/18.
//

#include <chrono>
#include <thread>

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"

#include "tento/base/Timestamp.hpp"
#include "tento/base/Duration.hpp"
using namespace tento;

int main() {
    Timestamp timestamp = Timestamp::Invalid();
    assert(!timestamp.IsValid());

    auto now = Timestamp::Now();
    fmt::print("now = {}\n", now);
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(2s);
    auto new_now = Timestamp::Now();
    fmt::print("now = {}\n", new_now);
    fmt::print("duration = {}\n", new_now.DurationSince(now));

    now = Timestamp::Now();
    auto duration = Duration::FromSecs(3);
    std::this_thread::sleep_for(3s);
    assert(now.Elapsed() >= duration);

    return 0;
}