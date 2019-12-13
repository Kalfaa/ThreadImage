cmake_minimum_required(VERSION 3.15)
project(ThreadImage C)
include_directories(${YOUR_DIRECTORY})

set(CMAKE_C_STANDARD 11)
set(SOURCES main.c lib/bitmap.c lib/bitmap.h)
add_executable(test ${SOURCES})
find_package (Threads)

add_executable(ThreadImage ${SOURCES})

target_link_libraries (ThreadImage ${CMAKE_THREAD_LIBS_INIT})
