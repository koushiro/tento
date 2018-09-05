//
// Created by koushiro on 9/4/18.
//

#include "tento/net/EventLoop.hpp"

#include "tento/base/Logger.hpp"
#include "tento/base/OS.hpp"

using namespace tento;
using namespace tento::net;

int main() {
    Logger logger(Logger::LogKind::Both);   /// file log?????

    LOG_INFO("main(): tid = {}", thread_id());
    EventLoop mainLoop;

    auto thread = std::thread([]() {
        LOG_INFO("thread(): tid = {}", thread_id());
        EventLoop threadLoop;
        threadLoop.RunAfter(Duration::FromSecs(5), [&]() {
            LOG_INFO("RunAfter callback tid = {}", thread_id());
//           EventLoop anotherLoop;
        });
        threadLoop.Run();
    });

    mainLoop.Run();

    return 0;
}