cmake_minimum_required(VERSION 3.29)
project(TP_SO_24_25 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(TP_SO_24_25
    feed.c
    feed.h
    manager.c
    manager.h)
