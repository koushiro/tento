//
// Created by koushiro on 9/4/18.
//

#include <chrono>
using namespace std::chrono_literals;

#include "tento/base/Logger.hpp"
#include "tento/base/Thread.hpp"
#include "tento/net/EventLoopThread.hpp"

using namespace tento;
using namespace tento::net;

int main() {
    Logger logger(Logger::LogKind::Both);

    auto print = [](EventLoop* loop) {
        LOG_INFO("print() callback, loop = {}", (void*)loop);
    };

    auto quit = [](EventLoop* loop) {
        LOG_INFO("quit() callback, loop = {}", (void*)loop);
        loop->Quit();
    };

    print(nullptr);

    {
        EventLoopThread thread;
    }

    {   /// Quit() was called in dtor of EventLoopThread.
        EventLoopThread thread;
        auto loop = thread.GetLoop();
        loop->RunInLoop(std::bind(print, loop));
        std::this_thread::sleep_for(500ms);
    }

    {   /// Quit() was called in Stop() and dtor of EventLoopThread
        EventLoopThread thread;
        auto loop = thread.GetLoop();
        loop->RunInLoop(std::bind(print, loop));
        thread.Stop();
        std::this_thread::sleep_for(500ms);
    }

    {   /// Quit() was called in quit() and dtor of EventLoopThread
        EventLoopThread thread;
        auto loop = thread.GetLoop();
        loop->QueueInLoop(std::bind(quit, loop));
        std::this_thread::sleep_for(500ms);
    }

    {   /// Quit() was called in quit(), Stop() and dtor of EventLoopThread
        EventLoopThread thread;
        auto loop = thread.GetLoop();
        loop->QueueInLoop(std::bind(quit, loop));
        thread.Stop();
        std::this_thread::sleep_for(500ms);
    }

    return 0;
}