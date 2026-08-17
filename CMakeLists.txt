cmake_minimum_required(VERSION 3.10)
project(vm_practice LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra)

set(SOURCES src/arena.c src/file.c src/main.c)

add_library(linenoise STATIC external/linenoise/linenoise.c)
target_include_directories(linenoise PUBLIC external/linenoise)

add_library(csv STATIC external/libcsv/libcsv.c)
target_include_directories(csv PUBLIC external/libcsv)

add_executable(main.run ${SOURCES})
target_link_libraries(main.run PRIVATE linenoise)
target_link_libraries(main.run PRIVATE csv)
