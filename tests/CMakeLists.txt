include_directories(${PROJECT_SOURCE_DIR}/build/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build/bin)

add_executable(test01 test01.cpp)
target_link_libraries(test01 tento)
