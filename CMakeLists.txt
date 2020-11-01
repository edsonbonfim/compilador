cmake_minimum_required(VERSION 3.12)

project(COMP)

set(EXEC comp)
set(CMAKE_CXX_STANDARD 14)

file(GLOB SOURCES *.cpp)

add_executable(${EXEC} ${SOURCES})
