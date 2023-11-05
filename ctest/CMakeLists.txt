cmake_minimum_required(VERSION 3.26)

include_directories(../libs)
link_libraries(natural)

add_executable(ctest_natural natural.c test.h)
add_test(natural ctest_natural)