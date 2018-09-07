include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/deps/fmt/include)
include_directories(${PROJECT_SOURCE_DIR}/deps/spdlog/include)

set(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build/lib)

set(TENTO_BASE
        base/Common.hpp
        base/NonCopyable.hpp
        base/Copyable.hpp
        base/Logger.hpp
        base/Duration.hpp
        base/Timestamp.hpp
        base/Thread.hpp
        base/CountDownLatch.hpp
        )

set(TENTO_NET
        net/Alias.hpp
        net/ServerStatus.hpp
        net/Channel.cpp
        net/Channel.hpp
        net/EventLoop.cpp
        net/EventLoop.hpp
        net/EPoller.cpp
        net/EPoller.hpp
        net/Timer.cpp
        net/Timer.hpp
        net/TimerQueue.cpp
        net/TimerQueue.hpp
        net/EventLoopThread.cpp
        net/EventLoopThread.hpp
        net/EventLoopThreadPool.cpp
        net/EventLoopThreadPool.hpp
        net/Buffer.cpp
        net/Buffer.hpp
        net/Socket.cpp
        net/Socket.hpp
        net/Acceptor.cpp
        net/Acceptor.hpp
#        net/Connector.cpp
#        net/Connector.hpp
        )

set(TENTO_NET_TCP
#        net/tcp/TcpConnection.cpp
#        net/tcp/TcpConnection.hpp
#        net/tcp/TcpServer.cpp
#        net/tcp/TcpServer.hpp
#        net/tcp/TcpClient.cpp
#        net/tcp/TcpClient.hpp
        )

add_library(tento ${TENTO_BASE} ${TENTO_NET} ${TENTO_NET_TCP})
target_link_libraries(tento pthread)
set_target_properties(tento PROPERTIES OUTPUT_NAME "tento")
set_target_properties(tento PROPERTIES LINKER_LANGUAGE CXX)
