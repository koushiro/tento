//
// Created by koushiro on 8/31/18.
//

#pragma once

#include <cassert>
#include <condition_variable>
#include <mutex>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"

NAMESPACE_BEGIN(tento)

class CountDownLatch : public  NonCopyable {
public:
    explicit CountDownLatch(int count) : count_(count) {}
    ~CountDownLatch() = default;

    void CountDown() {
        assert(count_ > 0);
        std::unique_lock<std::mutex> lock(mutex_);
        --count_;
        if (count_ == 0) {
            cond_.notify_all();
        }
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ > 0) {
            cond_.wait(lock);
        }
    }

    int GetCount() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return count_;
    }

private:
    int count_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
};

NAMESPACE_END(tento)
