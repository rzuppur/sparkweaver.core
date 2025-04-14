cmake_minimum_required(VERSION 3.30)
project(sparkweaver_core)

set(CMAKE_CXX_STANDARD 20)

add_library(sparkweaver_core
        src/Node.cpp
        src/Node.h
        src/Builder.cpp
        src/Builder.h)
target_include_directories(sparkweaver_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(sparkweaver_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/nodes)
target_include_directories(sparkweaver_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
