cmake_minimum_required(VERSION 3.24)
project(BLT)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_STD "Build the BLT standard utilities." ON)
option(BUILD_PROFILING "Build the BLT profiler extension" ON)

if(${BUILD_STD})
    file(GLOB_RECURSE STD_FILES "${CMAKE_SOURCE_DIR}/src/blt/std/*.cpp")
else()
    set(STD_FILES "")
endif()

if(${BUILD_PROFILING})
    file(GLOB_RECURSE PROFILING_FILES "${CMAKE_SOURCE_DIR}/src/blt/profiling/*.cpp")
else()
    set(PROFILING_FILES "")
endif()

include_directories(include/)

add_library(BLT ${STD_FILES} ${PROFILING_FILES})
