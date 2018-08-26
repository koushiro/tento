#include <iostream>

#include <tento/base/Logger.hpp>
#include <tento/net/EventLoop.hpp>

using namespace tento;
using namespace tento::net;

void ThreadFunc() {
    LOG_TRACE("ThreadFunc(): tid = {}", std::this_thread::get_id());
    EventLoop loop;
    loop.Loop();
}

int main() {
    tento::InitBothLogger();

    EventLoop loop;
    std::thread t(ThreadFunc);

    loop.Loop();

    t.join();

    tento::DropAllLogger();
    return 0;
}