include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/deps/fmt/include)
include_directories(${PROJECT_SOURCE_DIR}/deps/spdlog/include)
include_directories(${PROJECT_SOURCE_DIR}/build/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/bin)

# Base classes tests
add_executable(base_count_down_latch_test base_count_down_latch_test.cpp)
target_link_libraries(base_count_down_latch_test tento)

add_executable(base_duration_test base_duration_test.cpp)
target_link_libraries(base_duration_test tento)

add_executable(base_logger_test base_logger_test.cpp)
target_link_libraries(base_logger_test tento)

add_executable(base_thread_test base_thread_test.cpp)
target_link_libraries(base_thread_test tento)

add_executable(base_timestamp_test base_timestamp_test.cpp)
target_link_libraries(base_timestamp_test tento)

# Net -- Reactor classes tests.
add_executable(net_event_loop_test net_event_loop_test.cpp)
target_link_libraries(net_event_loop_test tento)

add_executable(net_timer_test net_timer_test.cpp)
target_link_libraries(net_timer_test tento)

add_executable(net_event_loop_thread_test net_event_loop_thread_test.cpp)
target_link_libraries(net_event_loop_thread_test tento)

add_executable(net_event_loop_thread_pool_test net_event_loop_thread_pool_test.cpp)
target_link_libraries(net_event_loop_thread_pool_test tento)

add_executable(net_listener_test net_listener_test.cpp)
target_link_libraries(net_listener_test tento)