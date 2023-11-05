cmake_minimum_required(VERSION 3.26)
project(natural C)

set(CMAKE_C_STANDARD 11)

add_library(natural natural.c natural.h)
target_include_directories(natural PUBLIC .)