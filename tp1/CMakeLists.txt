cmake_minimum_required(VERSION 3.14)
project(tp1 C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(tp1 TP1.c)
target_link_libraries(tp1 Threads::Threads)