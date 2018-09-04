//
// Created by koushiro on 9/4/18.
//

#include "tento/net/EventLoop.hpp"

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"

#include "tento/base/Logger.hpp"
#include "tento/base/OS.hpp"

using namespace tento;
using namespace tento::net;

int main() {
    Logger logger(Logger::LogKind::Both);

    fmt::print("main(): tid = {}\n", thread_id());

    EventLoop mainLoop;
    auto thread = std::thread([]() {
        fmt::print("thread(): tid = {}\n", thread_id());
        EventLoop threadLoop;
        auto now = Timestamp::Now();
        threadLoop.RunAfter(Duration::FromSecs(5), [&]() {
           fmt::print("RunAfter callback tid = {}\n", thread_id());
//           EventLoop anotherLoop;
            fmt::print("{}\n", now.Elapsed());
        });
        threadLoop.Run();
    });

    mainLoop.Run();
    thread.join();

    return 0;
}