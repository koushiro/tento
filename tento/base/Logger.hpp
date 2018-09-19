//
// Created by koushiro on 8/24/18.
//

#pragma once

#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/fmt/ostr.h>

#include "tento/base/Common.hpp"
#include "tento/base/NonCopyable.hpp"

NAMESPACE_BEGIN(tento)

#define LOGGER_NAME "tento"

/// console log level - trace;  file log level - info.
class Logger : NonCopyable {
public:
    enum class LogKind {
        Console,
        BasicFile,
        Both,
    };

    explicit Logger(LogKind kind) {
        switch (kind) {
            case LogKind::Console:   InitConsoleLogger(); break;
            case LogKind::BasicFile: InitBasicLogger();   break;
            case LogKind::Both:      InitBothLogger();    break;
        }
    }

    ~Logger() {
        DropAllLogger();
    }

private:
    void InitConsoleLogger() {
        auto logger = spdlog::stdout_color_mt<spdlog::async_factory>(LOGGER_NAME);
        logger->set_level(spdlog::level::trace);
        logger->set_pattern("[%Y-%m-%d %T.%e] [%l] [tid=%t] %v");

        logger->info(">>>>>>>>>>>>> Start Console Logging <<<<<<<<<<<<<");
    }

    void InitBasicLogger() {
        try {
            auto logger = spdlog::basic_logger_mt<spdlog::async_factory>(
                LOGGER_NAME,
                "tento.log"
            );
            logger->set_level(spdlog::level::info);
            logger->set_pattern("[%Y-%m-%d %T.%e] [%l] [tid=%t] %v");

            logger->info(">>>>>>>>>>>>> Start Basic-File Logging <<<<<<<<<<<<<");

        } catch (spdlog::spdlog_ex& ex) {
            std::cout <<  "Log initialization failed: " << ex.what() << std::endl;
        }
    }

    void InitBothLogger() {
        try {
            spdlog::init_thread_pool(8192, 1);
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::trace);

            /// write log into file in 'wb' mode.
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("tento.log", true);
            file_sink->set_level(spdlog::level::info);

            std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};

            auto logger = std::make_shared<spdlog::async_logger>(
                LOGGER_NAME,
                sinks.begin(), sinks.end(),
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::overrun_oldest
            );
            logger->set_level(spdlog::level::trace);
            logger->set_pattern("[%Y-%m-%d %T.%e] [%l] [tid=%t] %v");

            logger->info(">>>>>>>>>>> Start Console & Basic-File logging <<<<<<<<<<<");

            spdlog::register_logger(logger);

        } catch (spdlog::spdlog_ex& ex) {
            std::cout <<  "Log initialization failed: " << ex.what() << std::endl;
        }
    }
    void DropAllLogger() {
        spdlog::drop_all();
    }
};

#define __FILENAME__ \
    (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __SUFFIX__(msg) \
    std::string(msg).append(" [")\
        .append(__FILENAME__)\
        .append("#").append(std::to_string(__LINE__))\
        .append("]").c_str()

#ifdef TURN_OFF_LOG
#define LOG_TRACE(msg, ...)
#define LOG_DEBUG(msg, ...)
#define LOG_INFO(...)
#define LOG_WARN(...)
#define LOG_ERROR(...)
#define LOG_CRITICAL(...)
#else
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
#define LOG_CRITICAL(...) \
    spdlog::get(LOGGER_NAME)->critical(__VA_ARGS__)
#endif

NAMESPACE_END(tento)