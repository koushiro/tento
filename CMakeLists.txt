cmake_minimum_required(VERSION 3.12)
project(tento)
set (PROJECT_VERSION_MAJOR 0)
set (PROJECT_VERSION_MINOR 1)

set(CMAKE_CXX_COMPILER "clang++")
#set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "--std=c++14 -g -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

add_subdirectory(tento)
add_subdirectory(tests)
#add_subdirectory(examples)