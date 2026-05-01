cmake_minimum_required(VERSION 3.20)

project(phase5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(phase5_headers INTERFACE)
target_include_directories(phase5_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(
    tests
    tests/unit_tests.cpp
)
target_link_libraries(tests PRIVATE phase5_headers)

add_executable(
    benchmarks
    src/benchmark.cpp
)
target_link_libraries(benchmarks PRIVATE phase5_headers)
