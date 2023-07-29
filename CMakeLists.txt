cmake_minimum_required(VERSION 3.5)
project(BLT VERSION 0.8.0)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_ADDRSAN "Enable the address sanitizer" OFF)
option(ENABLE_UBSAN "Enable the ub sanitizer" OFF)
option(ENABLE_TSAN "Enable the thread data race sanitizer" OFF)
option(BUILD_STD "Build the BLT standard utilities." ON)
option(BUILD_PROFILING "Build the BLT profiler extension" ON)
option(BUILD_NBT "Build the BLT NBT + eNBT extension" ON)
option(BUILD_PARSE "Build the BLT parsers" ON)
option(BUILD_TESTS "Build the BLT test set" OFF)
option(BLT_DISABLE_LOGGING "Disable blt::logging (all macros and will safely disable logging function!)" OFF)
option(BLT_DISABLE_TRACE "Disable blt::logging BLT_TRACE macro" OFF)
option(BLT_DISABLE_DEBUG "Disable blt::logging BLT_DEBUG macro" OFF)
option(BLT_DISABLE_INFO "Disable blt::logging BLT_INFO macro" OFF)
option(BLT_DISABLE_WARN "Disable blt::logging BLT_WARN macro" OFF)
option(BLT_DISABLE_ERROR "Disable blt::logging BLT_ERROR macro" OFF)
option(BLT_DISABLE_FATAL "Disable blt::logging BLT_FATAL macro" OFF)

configure_file(include/blt/config.h.in config/blt/config.h @ONLY)

if(${BUILD_STD} OR ${BUILD_PROFILING})
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

if(${BUILD_PARSE})
    file(GLOB_RECURSE PARSE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/parse/*.cpp")
else()
    set(PARSE_FILES "")
endif()

if(EXISTS libraries/parallel-hashmap)
    include_directories(libraries/parallel-hashmap)
endif()

#include zlib if the user has it.
find_package(ZLIB QUIET)

if (${ZLIB_FOUND})
    include_directories(${ZLIB_INCLUDE_DIRS})
else()
    message("ZLIB was not found, this is fine however if you wish you use gzip with NBT it is required.")
endif()

include_directories(include/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/config/)

message("Standard Files ${STD_FILES}")
message("Profiler Files ${PROFILING_FILES}")
message("Source: ${CMAKE_SOURCE_DIR}")
message("Current Source: ${CMAKE_CURRENT_SOURCE_DIR}")

add_library(BLT ${STD_FILES} ${PROFILING_FILES} ${NBT_FILES} ${PARSE_FILES})

target_include_directories(BLT PUBLIC include/)
target_include_directories(BLT PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/config/)
if(${ZLIB_FOUND})
     target_link_libraries(BLT PUBLIC ZLIB::ZLIB)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/libraries/parallel-hashmap)
    message("Including phmap")
    target_include_directories(BLT PUBLIC libraries/parallel-hashmap)
endif()

if(MSVC)
    #target_compile_options(BLT PRIVATE /W4)
else()
    # perhaps we should warn on unused variables, but BLT will have lots of them.
    target_compile_options(BLT PRIVATE -Wall -Wextra -Wpedantic)
endif()

message("BLT ${CMAKE_PROJECT_VERSION} Successfully included!")

if(${BUILD_TESTS})
    project(BLT_TESTS)

    file(GLOB_RECURSE TESTS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/tests/*.cpp")

    add_executable(BLT_TESTS ${TESTS_FILES})

    target_link_libraries(BLT_TESTS PUBLIC BLT)

    if(MSVC)
        #target_compile_options(BLT_TESTS PRIVATE /W4)
        if(${CMAKE_BUILD_TYPE} MATCHES Debug)
            target_link_options(BLT_TESTS PRIVATE /DEBUG)
        endif()
    else()
        target_compile_options(BLT_TESTS PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if (${ENABLE_ADDRSAN} MATCHES ON)
        target_compile_options(BLT_TESTS PRIVATE -fsanitize=address)
        target_link_options(BLT_TESTS PRIVATE -fsanitize=address)
    endif ()

    if (${ENABLE_UBSAN} MATCHES ON)
        target_compile_options(BLT_TESTS PRIVATE -fsanitize=undefined)
        target_link_options(BLT_TESTS PRIVATE -fsanitize=undefined)
    endif ()

    if (${ENABLE_TSAN} MATCHES ON)
        target_compile_options(BLT_TESTS PRIVATE -fsanitize=thread)
        target_link_options(BLT_TESTS PRIVATE -fsanitize=thread)
    endif ()

    message("BLT tests included!")
endif()
