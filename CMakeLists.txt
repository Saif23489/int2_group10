cmake_minimum_required(VERSION 3.27)
project(untitled1 C)

set(CMAKE_C_STANDARD 11)


file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/files.c/*.c")

include_directories(files.h/)

add_executable(untitled1 ${SOURCES}
        files.h/test.h
        files.c/test.c
        files.h/tree.h
        files.c/tree.c
        files.c/loc.c
        files.c/main.c)