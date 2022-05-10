cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(picalc-benchmark VERSION 0.1.0 LANGUAGES CXX C)

find_package(benchmark REQUIRED)

set(SOURCE_FILES
	profiler.cc)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
PRIVATE
	benchmark::benchmark benchmark::benchmark_main)