# cmake_minimum_required(VERSION <specify CMake version here>)
project(tento)

set(CMAKE_CXX_STANDARD 14)

set(TENTO_BASE
        tento/base/Common.hpp
        tento/base/NonCopyable.hpp
        tento/base/Copyable.hpp
        tento/base/Duration.cpp
        tento/base/Duration.hpp
        tento/base/Timestamp.hpp
        tento/base/Logger.hpp
        )

set(TENTO_NET
        tento/net/Buffer.cpp
        tento/net/Buffer.hpp
        tento/net/Timer.cpp
        tento/net/Timer.hpp
        tento/net/TimerQueue.cpp
        tento/net/TimerQueue.hpp
        tento/net/EPoller.cpp
        tento/net/EPoller.hpp
        tento/net/Channel.cpp
        tento/net/Channel.hpp
        tento/net/EventLoop.cpp
        tento/net/EventLoop.hpp
        tento/net/EventLoopThread.cpp
        tento/net/EventLoopThread.hpp
        tento/net/EventLoopThreadPool.cpp
        tento/net/EventLoopThreadPool.hpp
        tento/net/Acceptor.cpp
        tento/net/Acceptor.hpp
        tento/net/Connector.cpp
        tento/net/Connector.hpp
        )

set(TENTO_NET_TCP
        tento/net/tcp/TcpConnection.cpp
        tento/net/tcp/TcpConnection.hpp
        tento/net/tcp/TcpClient.cpp
        tento/net/tcp/TcpClient.hpp
        tento/net/tcp/TcpServer.cpp
        tento/net/tcp/TcpServer.hpp
        )

set(TENTO_NET_UDP

        )

set(TENTO_NET_HTTP

        )

include_directories(tento/../)
include_directories(ext/fmt/include)
include_directories(ext/spdlog/include)

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