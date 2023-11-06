cmake_minimum_required(VERSION 3.26)
project(Math C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_executable(Math main.c)

add_subdirectory(libs/natural)
target_link_libraries(Math natural)

enable_testing()
add_subdirectory(ctest)