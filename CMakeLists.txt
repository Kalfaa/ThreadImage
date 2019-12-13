cmake_minimum_required(VERSION 3.15)
project(ThreadImage C)

set(CMAKE_C_STANDARD 11)

find_package (Threads)


add_executable(ThreadImage main.c)

target_link_libraries (ThreadImage ${CMAKE_THREAD_LIBS_INIT})
