cmake_minimum_required(VERSION 3.16)
project(memory_manager CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(memory_manager
    src/allocator.cpp
    src/main.cpp
)

target_include_directories(memory_manager PRIVATE include)
target_compile_options(memory_manager PRIVATE -Wall -Wextra -Wpedantic -g)
