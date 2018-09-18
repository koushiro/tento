//
// Created by koushiro on 9/18/18.
//

#include "tento/base/Thread.hpp"
#include "tento/base/Logger.hpp"

using namespace tento;

int main() {
    Logger logger(Logger::LogKind::Both);

    LOG_INFO("main(): tid = {}", thread_id());

    auto thread = Thread([]() {
        LOG_INFO("thread(): tid = {}", thread_id());
    });
    
    return 0;
}