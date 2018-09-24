//
// Created by koushiro on 9/20/18.
//

#include "gtest/gtest.h"

#include "tento/base/Logger.hpp"
#include "tento/net/Socket.hpp"
using namespace tento;
using namespace tento::net;

class SocketTest : public testing::Test {
protected:
    Logger logger{Logger::LogKind::BasicFile};
    Socket sock1;
    Socket sock2;
};

TEST_F(SocketTest, SockAddr) {
    SockAddr addr1{12345};
    EXPECT_EQ(addr1.ToIp(), "127.0.0.1");
    EXPECT_EQ(addr1.ToPort(), 12345);
    EXPECT_EQ(addr1.ToIpAndPort(), "127.0.0.1:12345");
    addr1 = SockAddr{12345, false};
    EXPECT_EQ(addr1.ToIp(), "0.0.0.0");
    EXPECT_EQ(addr1.ToPort(), 12345);
    EXPECT_EQ(addr1.ToIpAndPort(), "0.0.0.0:12345");

    SockAddr addr2{"127.0.0.1", 23456};
    EXPECT_EQ(addr2.ToIp(), "127.0.0.1");
    EXPECT_EQ(addr2.ToPort(), 23456);
    EXPECT_EQ(addr2.ToIpAndPort(), "127.0.0.1:23456");

    SockAddr addr3{"192.168.0.1:34567"};
    EXPECT_EQ(addr3.ToIp(), "192.168.0.1");
    EXPECT_EQ(addr3.ToPort(), 34567);
    EXPECT_EQ(addr3.ToIpAndPort(), "192.168.0.1:34567");
}

TEST_F(SocketTest, Socket) {
    auto sock1Fd = sock1.Fd();
    auto sock2Fd = sock2.Fd();
    EXPECT_NE(sock1Fd, INVALID_SOCKET);
    EXPECT_NE(sock2Fd, INVALID_SOCKET);

    Socket sock3{sock2Fd};
    EXPECT_EQ(sock3.Fd(), sock2Fd);

    sock2 = std::move(sock1);
    EXPECT_EQ(sock1.Fd(), INVALID_SOCKET);
    EXPECT_EQ(sock2.Fd(), sock1Fd);
}