cmake_minimum_required(VERSION 3.10)
project(HFT_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_library(hft_core
    src/MarketData.cpp
    src/TradeLogger.cpp
)

add_executable(hft_app src/main.cpp)
target_link_libraries(hft_app PRIVATE hft_core)

add_executable(test_latency test/test_latency.cpp)
target_link_libraries(test_latency PRIVATE hft_core)
