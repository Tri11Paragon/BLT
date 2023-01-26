cmake_minimum_required(VERSION 3.24)
project(BLT)

set(CMAKE_PROJECT_VERSION 0.2a)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_STD "Build the BLT standard utilities." ON)
option(BUILD_PROFILING "Build the BLT profiler extension" ON)
option(BUILD_NBT "Build the BLT NBT + eNBT extension" ON)
option(BUILD_TESTS "Build the BLT test set" OFF)

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

if(${BUILD_NBT})
    file(GLOB_RECURSE NBT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/nbt/*.cpp")
else()
    set(NBT_FILES "")
endif()

#include parallel hashmaps if the user decided to download the submodule.
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap/CMakeLists.txt)
    set(USE_PHMAPS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap/)
endif()

include_directories(include/)

message("Standard Files ${STD_FILES}")
message("Profiler Files ${PROFILING_FILES}")
message("Source: ${CMAKE_SOURCE_DIR}")
message("Current Source: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(BLT ${STD_FILES} ${PROFILING_FILES} ${NBT_FILES})
target_include_directories(BLT PUBLIC include/)

if(${USE_PHMAPS})
     target_link_libraries(BLT phmap)
endif()
message("BLT ${CMAKE_PROJECT_VERSION} Successfully included!")

if(${BUILD_TESTS})
    project(BLT_TESTS)

    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)

    file(GLOB_RECURSE TESTS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.cpp")

    add_executable(BLT_TESTS ${TESTS_FILES})

    target_link_libraries(BLT_TESTS BLT)
    message("BLT tests included!")
endif()
