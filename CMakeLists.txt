cmake_minimum_required(VERSION 3.10)
project(vm_practice LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

set(SOURCES src/arena.c src/main.c)

add_executable(main.run ${SOURCES})
