cmake_minimum_required(VERSION 3.20)
project(VarjusAPI LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Collect all .cpp files from the entire project
file(GLOB_RECURSE API_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../fs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../linter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../runtime/*.cpp"
)

# Filter out CMake's compiler detection files
list(FILTER API_SOURCES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp$")

# Create a static library
add_library(VarjusAPI STATIC ${API_SOURCES})

# Set include directories
target_include_directories(VarjusAPI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")

# Compiler options
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(VarjusAPI PRIVATE -Wall -Wextra -Werror -pedantic -O3)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(VarjusAPI PRIVATE /W4 /WX)
endif()
