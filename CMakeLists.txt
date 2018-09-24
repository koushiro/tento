cmake_minimum_required(VERSION 3.12)
project(tento)
set(TENTO_VERSION 0.1.0)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "--std=c++14 -g -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

include_directories(${PROJECT_SOURCE_DIR})

# ------------------------------------------------------------------------------
# 3rd party dependencies
# ------------------------------------------------------------------------------
set(DEPS_DIR ${PROJECT_SOURCE_DIR}/deps)
# include fmt/include directory
include_directories(${DEPS_DIR}/fmt/include)
# include spdlog/include directory
include_directories(${DEPS_DIR}/spdlog/include)
# Add the google test subdirectory
add_subdirectory(${DEPS_DIR}/googletest)
# include googltest/include directory.
include_directories(${DEPS_DIR}/googletest/googletest/include)
# include googlmock/include drectory
include_directories(${DEPS_DIR}/googletest/googlemock/include)

# ------------------------------------------------------------------------------
# tento
# ------------------------------------------------------------------------------
add_subdirectory(tento)

# ------------------------------------------------------------------------------
# tests
# ------------------------------------------------------------------------------
add_subdirectory(tests)