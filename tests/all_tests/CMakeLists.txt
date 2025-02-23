# Minimum CMake version required
cmake_minimum_required(VERSION 3.20)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Project name and version
project(Varjus VERSION 1.0 LANGUAGES CXX)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Recursively add all .cpp files from its subdirectories
# Current directory: \Varjus\tests\basics
file(GLOB_RECURSE PROJECT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../api/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../fs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../linter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../runtime/*.cpp"
)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp$")

add_executable(Varjus ${PROJECT_SOURCES})

# Add the root directory as an include directory
target_include_directories(Varjus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../")

# define RUNNING_TESTS
target_compile_definitions(Varjus PRIVATE RUNNING_TESTS)

add_custom_command(TARGET Varjus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Varjus> "${CMAKE_CURRENT_SOURCE_DIR}/"
)

message(STATUS "Building tests...")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Varjus PRIVATE -Wall -Wextra -Werror -pedantic -O3)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Varjus PRIVATE /W4 /WX /external:W3)
endif()