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

#define LOGGER_NAME "tento"

void InitConsoleLogger() {
    auto logger = spdlog::stdout_color_mt(LOGGER_NAME);
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%Y-%m-%d %T.%e] [%l] [tid=%t] %v");

    logger->info(">>>>>>>>>>>>> Start Console Logging <<<<<<<<<<<<<");
}

void InitBasicLogger() {
    auto logger = spdlog::basic_logger_mt(LOGGER_NAME, "logs/tento.log");
    logger->set_level(spdlog::level::info);
    logger->set_pattern("[%Y-%m-%d %T.%e] [%l] [tid=%t] %v");

    logger->info(">>>>>>>>>>>>> Start Basic-File Logging <<<<<<<<<<<<<");
}

void InitBothLogger() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/tento.log", true);
    file_sink->set_level(spdlog::level::info);

    auto sinks = spdlog::sinks_init_list { console_sink, file_sink };

    auto logger = std::make_shared<spdlog::logger>(LOGGER_NAME, sinks);
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("[%Y-%m-%d %T.%e] [%l] [tid=%t] %v");

    spdlog::register_logger(logger);

    logger->info(">>>>>>>>>>> Start Console & Basic-File logging <<<<<<<<<<<");
}

//void InitAsyncLogger() {
//
//}

#define LOG_TRACE(...) spdlog::get(LOGGER_NAME)->trace(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::get(LOGGER_NAME)->debug(__VA_ARGS__)
#define LOG_INFO(...) spdlog::get(LOGGER_NAME)->info(__VA_ARGS__)
#define LOG_WARN(...) spdlog::get(LOGGER_NAME)->warn(__VA_ARGS__)
#define LOG_ERROR(...) spdlog::get(LOGGER_NAME)->error(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::get(LOGGER_NAME)->critical(__VA_ARGS__)

NAMESPACE_END(tento)