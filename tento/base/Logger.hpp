//
// Created by koushiro on 8/24/18.
//

#pragma once

#include <tento/base/Common.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

NAMESPACE_BEGIN(tento)

class Logger {
public:
    Logger() {

    }

    ~Logger() {

    }

private:

};

#define LOG_TRACE
#define LOG_DEBUG
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR
#define LOG_CRITICAL

NAMESPACE_END(tento)