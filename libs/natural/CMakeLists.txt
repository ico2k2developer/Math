cmake_minimum_required(VERSION 3.26)
project(natural C)

set(CMAKE_C_STANDARD 11)

# Use SHARED like in
# add_library(natural SHARED natural.c natural.h)
# to compile the library as DLL file

add_library(natural natural.c natural.h)
target_include_directories(natural PUBLIC .)