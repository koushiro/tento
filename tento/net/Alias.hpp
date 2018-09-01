//
// Created by koushiro on 8/28/18.
//

#pragma once

#include <functional>
#include <vector>

#include "tento/base/Common.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

using Callback = std::function<void()>;

using TimerCallback = std::function<void()>;

using EventCallback = std::function<void()>;

class Channel;
using ChannelList = std::vector<Channel*>;

class Timer;
using TimerId = std::pair<uint64_t, Timer*>;

NAMESPACE_END(net)
NAMESPACE_END(tento)