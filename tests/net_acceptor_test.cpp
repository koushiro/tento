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
#include "tento/net/Acceptor.hpp"

using namespace tento;
using namespace tento::net;

int main() {
    Logger logger(Logger::LogKind::Both);

    fmt::print("main(): tid = {}\n", thread_id());
    SockAddr listenAddr(9981);
    EventLoop loop;
    Acceptor acceptor(&loop, listenAddr);
    acceptor.SetNewConnectionCallback([](Socket sock, const SockAddr& peerAddr) {
        fmt::print("accepted a new connection from {}\n", peerAddr.ToIpAndPort());
        std::string str("How are you?\n");
        write(sock.Fd(), str.c_str(), str.length());
    });
    acceptor.Listen();
    assert(acceptor.IsListening());

    loop.Run();

    return 0;
}