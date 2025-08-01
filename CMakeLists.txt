cmake_minimum_required(VERSION 3.25)

project(work VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(work cpp/main.cpp cpp/epoll.cpp cpp/result.cpp cpp/io_uring.cpp cpp/fd.cpp)
target_compile_options(work PRIVATE -fno-exceptions -fno-rtti)
target_include_directories(work PRIVATE third_party/expected/include)
target_link_libraries(work PRIVATE -luring -lfmt)
