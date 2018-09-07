//
// Created by koushiro on 9/4/18.
//

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"

#include "tento/base/Logger.hpp"
#include "tento/base/Thread.hpp"
#include "tento/net/EventLoop.hpp"

using namespace tento;
using namespace tento::net;

int cnt = 0;
EventLoop* gLoop;

int main() {
    Logger logger(Logger::LogKind::Both);

    fmt::print("tid = {}\n", thread_id());

    auto print = [](const char* msg) {
        LOG_TRACE("{}", msg);
        if (++cnt == 15) {
            gLoop->Quit();
        }
    };

    auto cancel = [](TimerId timerId) {
        gLoop->CancelTimer(timerId);
        LOG_TRACE("cancelled timer, id = {}", timerId.first);
    };

    EventLoop loop;
    gLoop = &loop;

    print("main");

    loop.RunAfter(Duration::FromMillis(1000), std::bind(print, "once1"));
    loop.RunAfter(Duration::FromMillis(1500), std::bind(print, "once1.5"));
    loop.RunEvery(Duration::FromMillis(2000), std::bind(print, "every2"));
    loop.RunAfter(Duration::FromMillis(2500), std::bind(print, "once2.5"));
    TimerId t3 = loop.RunEvery(Duration::FromMillis(3000), std::bind(print, "every3"));
    loop.RunAfter(Duration::FromMillis(3500), std::bind(print, "once3.5"));

    TimerId t4 = loop.RunAfter(Duration::FromMillis(4000), std::bind(print, "once4"));
    loop.RunAfter(Duration::FromMillis(3900), std::bind(cancel, t4));
    loop.RunAfter(Duration::FromMillis(4100), std::bind(cancel, t4));

    loop.RunAfter(Duration::FromMillis(9001), std::bind(cancel, t3));

    loop.Run();
    print("main loop exits");

    return 0;
}
