cmake_minimum_required(VERSION 3.20)
include(cmake/color.cmake)

set(BLT_VERSION 0.17.20)
set(BLT_TEST_VERSION 0.0.1)

set(BLT_TARGET BLT)

project(BLT VERSION ${BLT_VERSION})

set(CMAKE_CXX_STANDARD 17)

option(ENABLE_ADDRSAN "Enable the address sanitizer" OFF)
option(ENABLE_UBSAN "Enable the ub sanitizer" OFF)
option(ENABLE_TSAN "Enable the thread data race sanitizer" OFF)

option(BUILD_STD "Build the BLT standard utilities." ON)
option(BUILD_PROFILING "Build the BLT profiler extension" ON)
option(BUILD_FS "Build the BLT FS utilities including the NBT + eNBT extension" ON)
option(BUILD_PARSE "Build the BLT parsers" ON)

option(BUILD_TESTS "Build the BLT test set" OFF)

option(BLT_DISABLE_STATS "Disable tracking stats in certain objects. Enabling this will cause stat functions to return 0" OFF)
option(BLT_DISABLE_LOGGING "Disable blt::logging (all macros and will safely disable logging function!)" OFF)
option(BLT_DISABLE_TRACE "Disable blt::logging BLT_TRACE macro" OFF)
option(BLT_DISABLE_DEBUG "Disable blt::logging BLT_DEBUG macro" OFF)
option(BLT_DISABLE_INFO "Disable blt::logging BLT_INFO macro" OFF)
option(BLT_DISABLE_WARN "Disable blt::logging BLT_WARN macro" OFF)
option(BLT_DISABLE_ERROR "Disable blt::logging BLT_ERROR macro" OFF)
option(BLT_DISABLE_FATAL "Disable blt::logging BLT_FATAL macro" OFF)

if(${BLT_DISABLE_STATS})
    add_compile_definitions(BLT_DISABLE_STATS)
endif ()

configure_file(include/blt/config.h.in config/blt/config.h @ONLY)

message("Enabling library compilation")
if (${BUILD_STD} OR ${BUILD_PROFILING})
    message(STATUS "Building ${Yellow}standard${ColourReset} cxx files")
    file(GLOB_RECURSE STD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/std/*.cpp")
else ()
    set(STD_FILES "")
endif ()

if (${BUILD_PROFILING})
    message(STATUS "Building ${Yellow}profiling${ColourReset} cxx files")
    file(GLOB_RECURSE PROFILING_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/profiling/*.cpp")
else ()
    set(PROFILING_FILES "")
endif ()

if (${BUILD_FS})
    message(STATUS "Building ${Yellow}filesystem${ColourReset} cxx files")
    file(GLOB_RECURSE FS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/fs/*.cpp")
else ()
    set(FS_FILES "")
endif ()

if (${BUILD_PARSE})
    message(STATUS "Building ${Yellow}parser${ColourReset} cxx files")
    file(GLOB_RECURSE PARSE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/parse/*.cpp")
else ()
    set(PARSE_FILES "")
endif ()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
    message("Found Parallel Hashmaps library, using ${Yellow}phmap${ColourReset} over ${Red}std::unordered_map${ColourReset}")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
else()
    message("Parallel Hashmaps library not found! using ${Yellow}std::unordered_map${ColourReset}")
endif ()

#include zlib if the user has it.
find_package(ZLIB QUIET)

if (${ZLIB_FOUND})
    include_directories(${ZLIB_INCLUDE_DIRS})
else ()
    message("ZLIB was not found, this is fine however if you wish you use gzip with NBT it is required.")
endif ()

include_directories(include/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/config/)

add_library(${BLT_TARGET} ${STD_FILES} ${PROFILING_FILES} ${FS_FILES} ${PARSE_FILES})

string(REPLACE "+" "\\+" escaped_source ${CMAKE_CURRENT_SOURCE_DIR})
string(APPEND escaped_source "/src/blt/.*/")
list(TRANSFORM STD_FILES REPLACE ${escaped_source} "")
list(TRANSFORM PROFILING_FILES REPLACE ${escaped_source} "")
list(TRANSFORM FS_FILES REPLACE ${escaped_source} "")
list(TRANSFORM PARSE_FILES REPLACE ${escaped_source} "")
message("Standard Files ${Magenta}${STD_FILES}${ColourReset}")
message("Profiler Files ${Magenta}${PROFILING_FILES}${ColourReset}")
message("FS Files ${Magenta}${FS_FILES}${ColourReset}")
message("Parser Files ${Magenta}${PARSE_FILES}${ColourReset}")
message("Source: ${CMAKE_SOURCE_DIR}")
message("Current Source: ${CMAKE_CURRENT_SOURCE_DIR}")
message("Binary: ${CMAKE_BINARY_DIR}")
message("Current Binary: ${CMAKE_CURRENT_BINARY_DIR}")

if (${ZLIB_FOUND})
    target_link_libraries(${BLT_TARGET} PUBLIC ZLIB::ZLIB)
endif ()

include(cmake/warnings.cmake)

target_include_directories(${BLT_TARGET} PUBLIC include/)
target_include_directories(${BLT_TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/config/)
if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
    message("Including Parallel Hashmap directory")
    target_include_directories(${BLT_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
endif ()

message("BLT ${Yellow}${BLT_VERSION}${ColourReset} Successfully included!")

message("Installing to ${CMAKE_INSTALL_LIBDIR} with headers at ${CMAKE_INSTALL_INCLUDEDIR}")

file(GLOB_RECURSE BLT_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
foreach (S ${BLT_HEADER_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "" SO ${S})
    string(REGEX REPLACE "\/[A-Z|a-z|0-9|_|-]*\\.h" "/" SA ${SO})
    list(APPEND BLT_F_HEADERS ${SA})
    install(FILES ${S} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${SA})
endforeach ()

install(FILES ${CMAKE_BINARY_DIR}/config/blt/config.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blt/)

set_target_properties(${BLT_TARGET} PROPERTIES VERSION ${BLT_VERSION})
set_target_properties(${BLT_TARGET} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${BLT_TARGET}
        CONFIGURATIONS RelWithDebInfo
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (${BUILD_TESTS})
    message("Building test version ${BLT_TEST_VERSION}")
    project(BLT_TESTS VERSION ${BLT_TEST_VERSION})

    include_directories(tests/include)

    file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/src/*.cpp")

    message("Using files ${TEST_FILES}")

    add_executable(BLT_TESTS ${TEST_FILES})

    target_link_libraries(BLT_TESTS PRIVATE BLT)

    include(cmake/warnings.cmake)
    include(cmake/sanitizers.cmake)

    message("Built tests")
endif ()

project(BLT)