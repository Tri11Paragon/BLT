cmake_minimum_required(VERSION 3.5)

set(BLT_VERSION 0.10.1)
set(BLT_TEST_VERSION 0.0.1)

set(BLT_TARGET BLT)

project(BLT VERSION ${BLT_VERSION})

set(CMAKE_CXX_STANDARD 17)

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
    message("Building STD")
    file(GLOB_RECURSE STD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/std/*.cpp")
else()
    set(STD_FILES "")
endif()

if(${BUILD_PROFILING})
    message("Building Profiling")
    file(GLOB_RECURSE PROFILING_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/profiling/*.cpp")
else()
    message("We are not building profiling")
    set(PROFILING_FILES "")
endif()

if(${BUILD_NBT})
    message("Building NBT")
    file(GLOB_RECURSE NBT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/nbt/*.cpp")
else()
    set(NBT_FILES "")
endif()

if(${BUILD_PARSE})
    message("Building Parsers")
    file(GLOB_RECURSE PARSE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/blt/parse/*.cpp")
else()
    set(PARSE_FILES "")
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
    message("Found Parallel Hashmaps")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
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

add_library(${BLT_TARGET} SHARED ${STD_FILES} ${PROFILING_FILES} ${NBT_FILES} ${PARSE_FILES})

target_include_directories(${BLT_TARGET} PUBLIC include/)
target_include_directories(${BLT_TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/config/)
if(${ZLIB_FOUND})
     target_link_libraries(${BLT_TARGET} PUBLIC ZLIB::ZLIB)
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
    message("Including phmap")
    target_include_directories(${BLT_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libraries/parallel-hashmap)
endif()

if(MSVC)
    #target_compile_options(${BLT_TARGET} PRIVATE /W4)
else()
    # perhaps we should warn on unused variables, but BLT will have lots of them.
    target_compile_options(${BLT_TARGET} PRIVATE -Wall -Wextra -Wpedantic)
    target_link_options(${BLT_TARGET} PUBLIC -rdynamic)
endif()

message("BLT ${CMAKE_PROJECT_VERSION} Successfully included!")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    include(GNUInstallDirs)
endif()

message("Installing to ${CMAKE_INSTALL_LIBDIR} with headers at ${CMAKE_INSTALL_INCLUDEDIR}")

file(GLOB_RECURSE BLT_HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
foreach(S ${BLT_HEADER_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/include/" "" SO ${S})
    string(REGEX REPLACE "\/[A-Z|a-z|0-9|_|-]*\\.h" "/" SA ${SO})
    list(APPEND BLT_F_HEADERS ${SA})
    install(FILES ${S} DESTINATION /usr/${CMAKE_INSTALL_INCLUDEDIR}/${SA})
endforeach ()

install(FILES ${CMAKE_BINARY_DIR}/config/blt/config.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/blt/)

set_target_properties(${BLT_TARGET} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${BLT_TARGET} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

install(TARGETS ${BLT_TARGET}
        CONFIGURATIONS RelWithDebInfo
        LIBRARY DESTINATION /usr/${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION /usr/${CMAKE_INSTALL_INCLUDEDIR})

if(${BUILD_TESTS})
    message("Building test")
    project(BLT_TESTS VERSION ${BLT_TEST_VERSION})

    include_directories(tests/include)

    file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/src/*.cpp")

    message("Using files ${TEST_FILES}")

    add_executable(BLT_TESTS ${TEST_FILES})

    target_link_libraries(BLT_TESTS BLT)

    if(MSVC)

    else()
        target_compile_options(BLT_TESTS PRIVATE -Wall -Werror -Wpedantic -Wno-comment)
        target_link_options(BLT_TESTS PRIVATE -Wall -Werror -Wpedantic -Wno-comment)
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
    message("Built tests")
endif()

project(BLT)