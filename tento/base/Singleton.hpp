//
// Created by koushiro on 8/17/18.
//

#pragma once

#include <tento/base/Common.hpp>
#include <tento/base/NonCopyable.hpp>

NAMESPACE_BEGIN(tento)

template<typename T>
class Singleton : public NonCopyable {
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
