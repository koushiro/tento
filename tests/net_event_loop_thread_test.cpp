//
// Created by koushiro on 9/4/18.
//

#include <chrono>
using namespace std::chrono_literals;

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ostream.h"

#include "tento/base/Logger.hpp"
#include "tento/base/OS.hpp"
#include "tento/net/EventLoopThread.hpp"

using namespace tento;
using namespace tento::net;

int main() {
    Logger logger(Logger::LogKind::Both);

    auto print = [](EventLoop* loop) {
        fmt::print("tid = {}, loop = {}\n", thread_id(), (void*)loop);
    };

    auto quit = [&](EventLoop* loop) {
        print(loop);
        loop->Quit();
    };

    print(nullptr);

    {
        EventLoopThread thread1;
    }

    {   /// Quit() was called in dtor of EventLoopThread.
        EventLoopThread thread2;
        auto loop = thread2.StartLoop();
        loop->RunInLoop(std::bind(print, loop));
        std::this_thread::sleep_for(500ms);
    }

    {   /// Quit() was called before dtor of EventLoopThread.
        /// terminate called without an active exception.
        /// See EventLoopThread::~EventLoopThread().
        EventLoopThread thread3;
        auto loop = thread3.StartLoop();
        loop->RunInLoop(std::bind(quit, loop));
        std::this_thread::sleep_for(500ms);
    }

    return 0;
}