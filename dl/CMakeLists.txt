cmake_minimum_required(VERSION 3.25)

project(DigitalLogicDesignLab)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(verilator HINTS $ENV{VERILATOR_ROOT})

file(GLOB_RECURSE SOURCE_FILES *.sv)

message(STATUS "================")
foreach(DUT_SOURCE ${SOURCE_FILES})
    string(REGEX MATCH "lab[0-9]+/.+\.sv" DUT_SOURCE_REL ${DUT_SOURCE})
    string(REGEX REPLACE "lab[0-9]+/(.+)\.sv" "\\1" DUT ${DUT_SOURCE_REL})
    if (EXISTS test/sim_${DUT}.cpp)
        message(STATUS "found module ${DUT}")
        add_executable(V${DUT} test/sim_${DUT}.cpp)
        verilate(V${DUT} SOURCES  ${DUT_SOURCE_REL} TRACE COVERAGE)
    else()
        message(STATUS "found module ${DUT} - skip")
    endif()
endforeach()
message(STATUS "================")
