//
// Created by koushiro on 9/4/18.
//

#include "tento/net/EventLoop.hpp"

#include "tento/base/Logger.hpp"
#include "tento/base/Thread.hpp"

using namespace tento;
using namespace tento::net;

int main() {
    Logger logger(Logger::LogKind::Both);

    LOG_INFO("main thread = {} start", thread_id());
    EventLoop mainLoop;
    mainLoop.RunAfter(Duration::FromSecs(20), [&mainLoop](){
        LOG_INFO("loop = {}, RunAfter callback",
            (void*)&mainLoop,  thread_id());
        mainLoop.Quit();
    });

    auto thread = Thread([]() {
        LOG_INFO("other thread = {} start", thread_id());
        EventLoop threadLoop;
        threadLoop.RunAfter(Duration::FromSecs(25), [&threadLoop]() {
            LOG_INFO("loop = {}, RunAfter callback",
                (void*)&threadLoop, thread_id());
            threadLoop.Quit();
        });
        threadLoop.Run();
        LOG_INFO("other thread = {} stop", thread_id());
    });

    mainLoop.Run();
    LOG_INFO("main thread = {} stop", thread_id());

    return 0;
}