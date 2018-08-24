//
// Created by koushiro on 8/17/18.
//

#pragma once

#include <tento/base/Common.hpp>

NAMESPACE_BEGIN(tento)

class NonCopyable {
public:
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

NAMESPACE_END(tento)
