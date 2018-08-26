//
// Created by koushiro on 8/25/18.
//

#include <tento/base/Logger.hpp>

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include <iostream>

NAMESPACE_BEGIN(tento)

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

        spdlog::register_logger(logger);

        logger->info(">>>>>>>>>>> Start Console & Basic-File logging <<<<<<<<<<<");

    } catch (spdlog::spdlog_ex& ex) {
        std::cout <<  "Log initialization failed: " << ex.what() << std::endl;
    }
}

void DropAllLogger() {
    spdlog::drop_all();
}

NAMESPACE_END(tento)