cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(os-good)

add_executable(shell case3/shell.cpp)
add_executable(sched case4/sched.cpp)
add_executable(p-graph case5/p-graph.cpp)
add_executable(p-space case6/p-space.cpp)
add_executable(thread case7/thread.cpp)

add_executable(gendata lab2/gendata.cpp)
add_executable(par-sort lab2/par-sort.cpp)

set(
    LAB3_SOURCES
    ${CMAKE_SOURCE_DIR}/lab3/gthr.c
    ${CMAKE_SOURCE_DIR}/lab3/gtswtch.S
)
add_custom_target(
    spawn-thread
    COMMAND gcc -o spawn-thread ${LAB3_SOURCES}
    SOURCES ${LAB3_SOURCES}
)
