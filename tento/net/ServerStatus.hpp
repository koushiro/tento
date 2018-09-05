//
// Created by koushiro on 9/5/18.
//

#pragma once

#include <atomic>
#include <string>

#include "tento/base/Common.hpp"

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

class ServerStatus {
public:
    enum class Status {
        kNull,
        kStarting,
        kRunning,
        kStopping,
        kStopped,
    };

    bool IsStarting() const { return status_.load() == Status::kStarting; }
    bool IsRunning()  const { return status_.load() == Status::kRunning;  }
    bool IsStopping() const { return status_.load() == Status::kStopping; }
    bool IsStopped()  const { return status_.load() == Status::kStopped;  }

    std::string StautsToString() const {
        switch (status_.load()) {
            case Status::kNull:     return "kNull";
            case Status::kStarting: return "kStarting";
            case Status::kRunning:  return "kRunning";
            case Status::kStopping: return "kStopping";
            case Status::kStopped:  return "kStopped";
        }
    }

protected:
    std::atomic<Status> status_ = { Status::kNull };
};

NAMESPACE_END(net)
NAMESPACE_END(tento)