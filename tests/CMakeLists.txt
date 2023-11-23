cmake_minimum_required(VERSION 3.1)
project(BLT_TESTS)

option(ENABLE_ADDRSAN "Enable the address sanitizer" ON)
option(ENABLE_UBSAN "Enable the ub sanitizer" ON)
option(ENABLE_TSAN "Enable the thread data race sanitizer" OFF)

set(CMAKE_CXX_STANDARD 17)

include_directories(include/)
file(GLOB_RECURSE PROJECT_BUILD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${PROJECT_BUILD_FILES})

message("Created Tests, Linking BLT")

target_link_libraries(${PROJECT_NAME} BLT)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wpedantic -Wno-comment)
target_link_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wpedantic -Wno-comment)

if (${ENABLE_ADDRSAN} MATCHES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif ()

if (${ENABLE_UBSAN} MATCHES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
endif ()

if (${ENABLE_TSAN} MATCHES ON)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=thread)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=thread)
endif ()