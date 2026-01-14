cmake_minimum_required(VERSION 4.2)
project(IPCQueue VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(ipc_lib INTERFACE)
target_include_directories(ipc_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(ipc_lib INTERFACE -Wall -Wextra -Wpedantic)
