# cmake_minimum_required(VERSION <specify CMake version here>)
project(tento)

set(CMAKE_CXX_STANDARD 14)

set(TENTO_BASE
        tento/base/NonCopyable.hpp
        tento/base/Copyable.hpp
        tento/base/Singleton.hpp
        tento/base/Duration.cpp tento/base/Duration.hpp
        tento/base/Timestamp.cpp tento/base/Timestamp.hpp
        tento/base/ThreadPool.cpp tento/base/ThreadPool.hpp
        )

set(TENTO_NET
        tento/net/Acceptor.cpp tento/net/Acceptor.hpp
        tento/net/Connector.cpp tento/net/Connector.hpp
        tento/net/EventLoop.cpp tento/net/EventLoop.hpp
        tento/net/Buffer.cpp tento/net/Buffer.hpp
        tento/net/EventLoopThread.cpp tento/net/EventLoopThread.hpp
        tento/net/EventLoopThreadPool.cpp tento/net/EventLoopThreadPool.hpp
        tento/net/Sockets.cpp tento/net/Sockets.hpp
        tento/net/Channel.cpp tento/net/Channel.hpp
        )

set(TENTO_NET_TCP

        tento/net/tcp/TcpConnection.cpp tento/net/tcp/TcpConnection.hpp tento/net/tcp/TcpClient.cpp tento/net/tcp/TcpClient.hpp tento/net/tcp/TcpServer.cpp tento/net/tcp/TcpServer.hpp)

set(TENTO_NET_UDP

        )

set(TENTO_NET_HTTP
)

add_executable(tento
        main.cpp
        ${TENTO_BASE}
        ${TENTO_NET}
        ${TENTO_NET_TCP}
        ${TENTO_NET_HTTP}
        )

target_link_libraries(tento
        pthread
        )