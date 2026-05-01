cmake_minimum_required(VERSION 3.20)

project(hw5_optimization LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(hw5_headers INTERFACE)
target_include_directories(hw5_headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(
    hash_table_benchmark
    benchmarks/hash_table_benchmark.cpp
    src/robin_hood_hash_table.cpp
)
target_link_libraries(hash_table_benchmark PRIVATE hw5_headers)

add_executable(
    hw5_benchmarks
    benchmarks/run_benchmarks.cpp
    src/robin_hood_hash_table.cpp
    src/order_priority_queue.cpp
    src/time_series_processor.cpp
    src/order_book.cpp
    src/benchmarks.cpp
)
target_link_libraries(hw5_benchmarks PRIVATE hw5_headers)

option(HW5_BUILD_STUBS "Build placeholder source files with no function implementations" OFF)
if(HW5_BUILD_STUBS)
    add_library(
        hw5_stubs
        STATIC
        src/robin_hood_hash_table.cpp
        src/order_priority_queue.cpp
        src/time_series_processor.cpp
        src/order_book.cpp
        src/benchmarks.cpp
    )
    target_link_libraries(hw5_stubs PUBLIC hw5_headers)
endif()
