cmake_minimum_required(VERSION 3.14)
project(tp2 C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(tp2 TP2.c)
add_executable(validator validator.c)
target_link_libraries(tp2 Threads::Threads)
