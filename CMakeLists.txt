cmake_minimum_required(VERSION 3.12)
project(COMP C)

set(CMAKE_C_STANDARD 99)

add_executable(
        COMP
        main.c
        lex.c
        lex.h
        tools.h
        tools.c
        parser.c parser.h)
