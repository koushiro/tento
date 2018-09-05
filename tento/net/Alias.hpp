//
// Created by koushiro on 8/28/18.
//

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "tento/base/Common.hpp"
#include "tento/base/Timestamp.hpp"
#include "tento/net/Buffer.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

using Callback = std::function<void ()>;

using TimerCallback = std::function<void ()>;

using EventCallback = std::function<void ()>;

class Channel;
using ChannelList = std::vector<Channel*>;

class Timer;
using TimerId = std::pair<uint64_t, Timer*>;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
using MessageCallback = std::function<void (const TcpConnectionPtr&, Buffer*, Timestamp)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;

NAMESPACE_END(net)
NAMESPACE_END(tento)