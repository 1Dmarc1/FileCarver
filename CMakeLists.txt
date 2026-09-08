cmake_minimum_required(VERSION 3.30)
project(FileCarver C)

set(CMAKE_C_STANDARD 23)

add_executable(FileCarver main.c
        filetype.c
        filetype.h
        carver.c
        carver.h
        parsers.c
        parsers.h)
