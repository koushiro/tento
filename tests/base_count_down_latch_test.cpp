//
// Created by koushiro on 9/3/18.
//

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"

#include "tento/base/CountDownLatch.hpp"
using namespace tento;

int main() {
    CountDownLatch latch(3);
    fmt::print("main tid = {}; count = {}\n",
               std::this_thread::get_id(), latch.GetCount());

    auto thread = std::thread([&]() {
        fmt::print("other tid = {}", std::this_thread::get_id());
        auto count = latch.GetCount();
        while (count > 0) {
            latch.CountDown();
            count = latch.GetCount();
            fmt::print("other tid = {}; count = {}\n",
                       std::this_thread::get_id(), count);
            std::this_thread::sleep_for(2s);
        }
    });
    thread.join();

    latch.Wait();
    fmt::print("main tid = {}; count = {}\n",
               std::this_thread::get_id(), latch.GetCount());
}