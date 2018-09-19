//
// Created by koushiro on 9/3/18.
//

#include "gtest/gtest.h"

#include "tento/base/Logger.hpp"
using namespace tento;

class LoggerTest : public testing::Test {
protected:
    Logger logger{Logger::LogKind::Both};
};

TEST_F(LoggerTest, Log) {
    LOG_TRACE("trace message {}", 1);
    LOG_TRACE("trace message {}", 2);
    LOG_DEBUG("debug message {}", 1);
    LOG_DEBUG("debug message {}", 2);
    LOG_INFO("info message");
    LOG_WARN("warn message");
    LOG_ERROR("error message");
    LOG_CRITICAL("critical message");
}