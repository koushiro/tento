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
#include "tento/net/Listener.hpp"

using namespace tento;
using namespace tento::net;

int main() {
    Logger logger(Logger::LogKind::Both);

    LOG_INFO("main thread");

    EventLoop loop;

    Listener listener {&loop, SockAddr(9981)};
    listener.SetNewConnectionCallback(
        [](Socket connSock, const SockAddr& remoteAddr) {
            std::string str("How are you?\n");
            write(connSock.Fd(), str.c_str(), str.length());
            LOG_INFO("Send packet over, remote address = {}", remoteAddr.ToIpAndPort());
        }
    );
    listener.Listen();
    assert(listener.IsListening());
    listener.Accept();

    loop.Run();

    return 0;
}