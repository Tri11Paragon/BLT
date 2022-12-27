cmake_minimum_required(VERSION 3.24)
project(BLT)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_STD "Build the BLT standard utilities." ON)
option(BUILD_PROFILING "Build the BLT profiler extension" ON)

if(${BUILD_STD})
    file(GLOB_RECURSE STD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/std/*.cpp")
else()
    set(STD_FILES "")
endif()

if(${BUILD_PROFILING})
    file(GLOB_RECURSE PROFILING_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/profiling/*.cpp")
else()
    set(PROFILING_FILES "")
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap/CMakeLists.txt)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap/)
    set(PHMAP_ENABLED ON)
endif()

configure_file(include/blt/config.h.in blt/config.h @ONLY)

message("Standard Files ${STD_FILES}")
message("Profiler Files ${PROFILING_FILES}")
message("Source: ${CMAKE_SOURCE_DIR}")
message("Current Source: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(BLT ${STD_FILES} ${PROFILING_FILES})
target_include_directories(BLT PUBLIC include/)
target_include_directories(BLT PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(BLT phmap)
