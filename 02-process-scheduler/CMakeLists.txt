cmake_minimum_required(VERSION 3.16)
project(process_scheduler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(process_scheduler
    src/scheduler.cpp
    src/main.cpp
)

target_include_directories(process_scheduler PRIVATE include)
target_compile_options(process_scheduler PRIVATE -Wall -Wextra -Wpedantic -g)
