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
        fmt::print("print(): tid = {}, loop = {}\n",
            thread_id(), (void*)loop);
    };

    auto quit = [&](EventLoop* loop) {
        print(loop);
        loop->Quit();
    };

    print(nullptr);

    {
        EventLoopThread thread;
    }

    {   /// Quit() was called in dtor of EventLoopThread.
        EventLoopThread thread;
        auto loop = thread.Start();
        loop->RunInLoop(std::bind(print, loop));
        std::this_thread::sleep_for(500ms);
    }

    {   /// Quit() was called in Stop() and dtor of EventLoopThread
        EventLoopThread thread;
        auto loop = thread.Start();
        loop->RunInLoop(std::bind(print, loop));
        thread.Stop();
        std::this_thread::sleep_for(500ms);
    }

    {   /// Quit() was called in quit() and dtor of EventLoopThread
        EventLoopThread thread;
        auto loop = thread.Start();
        loop->QueueInLoop(std::bind(quit, loop));
        std::this_thread::sleep_for(500ms);
    }

    {   /// Quit() was called in quit(), Stop() and dtor of EventLoopThread
        EventLoopThread thread;
        auto loop = thread.Start();
        loop->QueueInLoop(std::bind(quit, loop));
        thread.Stop();
        std::this_thread::sleep_for(500ms);
    }

    return 0;
}