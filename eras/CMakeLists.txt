cmake_minimum_required(VERSION 3.0)
project(eras C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-O3 -Wall -Wextra -Wpedantic -Werror -march=native)

add_executable(eras main.c)
