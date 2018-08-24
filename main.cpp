#include <iostream>

#include <tento/base/Logger.hpp>

int main() {
    tento::InitBothLogger();

    LOG_TRACE("Trace message {}", 1);
    LOG_DEBUG("Debug message {}", 2);
    LOG_INFO("Info message {}", 3);
    LOG_WARN("Warning message {}", 4);
    LOG_ERROR("Error message {}", 5);
    LOG_CRITICAL("Critical message {}", 6);

    return 0;
}