//
// Created by koushiro on 8/17/18.
//

#pragma once

#include "Common.hpp"

NAMESPACE_BEGIN(tento)

template<typename T>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T& GetInstance() {
        static T instance;
        return instance;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
};

NAMESPACE_END(tento)
