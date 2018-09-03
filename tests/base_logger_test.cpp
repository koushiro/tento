//
// Created by koushiro on 9/3/18.
//

#include "tento/base/Logger.hpp"
using namespace tento;

int main() {
    Logger logger(Logger::LogKind::Both);
    LOG_TRACE("trace message {}", 1);
    LOG_TRACE("trace message {}", 2);
    LOG_DEBUG("debug message {}", 1);
    LOG_DEBUG("debug message {}", 2);
    LOG_INFO("info message");
    LOG_WARN("warn message");
    LOG_ERROR("error message");
    LOG_CRITICAL("critical message");
    return 0;
}