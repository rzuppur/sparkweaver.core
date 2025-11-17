cmake_minimum_required(VERSION 3.30)
project(sparkweaver_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(sparkweaver_core
        src/Engine.cpp)

target_include_directories(sparkweaver_core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(sparkweaver_core_test test/demo.cpp)

target_link_libraries(sparkweaver_core_test PRIVATE sparkweaver_core)
