include_directories(${PROJECT_SOURCE_DIR}/target/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/target/bin)
set(DEPENDENT_LIBRARIES tento gtest gtest_main)

set(TEST_FILES
        # base
        test_count_down_latch.cpp
        test_duration.cpp
        test_logger.cpp
        test_thread.cpp
        test_timestamp.cpp

        # net
        test_listener.cpp
        test_event_loop.cpp
        test_event_loop_thread.cpp
        test_event_loop_thread_pool.cpp
        test_timer.cpp
        )

foreach(test_file ${TEST_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} ${DEPENDENT_LIBRARIES})
endforeach()