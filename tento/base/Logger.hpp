//
// Created by koushiro on 8/24/18.
//

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "tento/base/Common.hpp"

NAMESPACE_BEGIN(tento)

#define LOGGER_NAME "tento"

void InitConsoleLogger();
void InitBasicLogger();
void InitBothLogger();

void DropAllLogger();

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __SUFFIX__(msg) \
    std::string(msg).append(" [")\
        .append(__FILENAME__)\
        .append("#").append(std::to_string(__LINE__))\
        .append("]").c_str()

#define LOG_TRACE(msg, ...) \
    spdlog::get(LOGGER_NAME)->trace(__SUFFIX__(msg), __VA_ARGS__)
#define LOG_DEBUG(msg, ...) \
    spdlog::get(LOGGER_NAME)->debug(__SUFFIX__(msg), __VA_ARGS__)
#define LOG_INFO(...) \
    spdlog::get(LOGGER_NAME)->info(__VA_ARGS__)
#define LOG_WARN(...) \
    spdlog::get(LOGGER_NAME)->warn(__VA_ARGS__)
#define LOG_ERROR(...) \
    spdlog::get(LOGGER_NAME)->error(__VA_ARGS__)
#define LOG_FATAL(...) \
    spdlog::get(LOGGER_NAME)->critical(__VA_ARGS__)

NAMESPACE_END(tento)