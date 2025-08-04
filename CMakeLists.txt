cmake_minimum_required(VERSION 3.25)

project(
  work
  VERSION 0.1.0
  LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
  work
  cpp/main.cpp
  cpp/fd.cpp
  cpp/runner.cpp
  cpp/stopper.cpp
  cpp/signal_handler.cpp
  cpp/event_loop.cpp
  cpp/create.cpp)
target_compile_options(work PRIVATE -fno-exceptions -fno-rtti)
target_link_libraries(work PRIVATE -luring -lfmt)
