//
// Created by koushiro on 9/6/18.
//

#include <chrono>
using namespace std::chrono_literals;

#include "tento/base/Logger.hpp"
#include "tento/base/Thread.hpp"
#include "tento/net/EventLoopThreadPool.hpp"

using namespace tento;
using namespace tento::net;

void print(EventLoop* loop = nullptr) {
    LOG_TRACE("print(): tid = {}, loop = {}", thread_id(), (void*)loop);
}

int main() {
    Logger logger(Logger::LogKind::Both);

    print();

    EventLoop loop;
    loop.RunAfter(Duration::FromSecs(15), [&]() { return loop.Quit(); });

    {
        LOG_TRACE("Single thread, loop = {}", (void*)&loop);
        EventLoopThreadPool pool(&loop);
        assert(pool.GetNextLoop() == &loop);
        assert(pool.GetNextLoop() == &loop);
        assert(pool.GetNextLoop() == &loop);
    }

    {
        LOG_TRACE("Another thread", "");
        EventLoopThreadPool pool(&loop, 1);
        EventLoop* nextLoop = pool.GetNextLoop();
        assert(nextLoop);
        nextLoop->RunAfter(Duration::FromSecs(2), [=]() { return print(nextLoop); });
        assert(nextLoop != &loop);
        assert(nextLoop == pool.GetNextLoop());
        assert(nextLoop == pool.GetNextLoop());
        std::this_thread::sleep_for(3s);
    }

    {
        LOG_TRACE("Three threads", "");
        EventLoopThreadPool pool(&loop, 3);
        EventLoop* nextLoop = pool.GetNextLoop();
        nextLoop->RunInLoop([=]() { return print(nextLoop); });
        assert(nextLoop != &loop);
        assert(nextLoop != pool.GetNextLoop());
        assert(nextLoop != pool.GetNextLoop());
        assert(nextLoop == pool.GetNextLoop());
    }

    loop.Run();

    return 0;
}