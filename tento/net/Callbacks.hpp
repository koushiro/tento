//
// Created by koushiro on 8/28/18.
//

#pragma once

#include <functional>

#include "tento/base/Common.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

using Callback = std::function<void()>;

using TimerCallback = std::function<void()>;

using EventCallback = std::function<void()>;

NAMESPACE_END(net)
NAMESPACE_END(tento)