cmake_minimum_required(VERSION 3.0)
project(project1 C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_executable(project1 main.c)
