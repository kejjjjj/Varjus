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

# Recursively add all .cpp files
file(GLOB_RECURSE PROJECT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/linter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/*.cpp"
)

add_executable(Varjus ${PROJECT_SOURCES})

# Add the root directory as an include directory
target_include_directories(Varjus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building in Release mode")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(Varjus PRIVATE -Wall -Wextra -Werror -external:W3 -pedantic -O3)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(Varjus PRIVATE 
            /Wall /WX /external:W3
            /wd4514 # Unreferenced inline function has been removed
            /wd4626 # Assignment operator implicitly deleted
            /wd4820 # Padding added for alignment
            /wd5267 # Definition hides non-virtual function
            /wd4351 # Base class member initialized
            /wd5045 # Spectre mitigation info
            /wd4371 # Layout of base class has changed
        )
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(Varjus PRIVATE 
            -Wall -Wextra -Werror /external:W3
            -pedantic 
            -O0 -g  # Disable optimizations and enable debugging
            -fsanitize=address  # Enable AddressSanitizer
            -DDEBUG  # Enable Debug Assertions (you can define custom macros like DEBUG)
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(Varjus PRIVATE 
            /Wall /WX /external:W3
            /Od /D_DEBUG  # Disable optimizations and enable debug mode
            /ZI  # Enable Debug Information
            /RTC1  # Enable runtime checks (Debug mode)
            /DDEBUG  # Enable Debug Assertions

            /wd4514 # Unreferenced inline function has been removed
            /wd4626 # Assignment operator implicitly deleted
            /wd4820 # Padding added for alignment
            /wd5267 # Definition hides non-virtual function
            /wd4351 # Base class member initialized
            /wd5045 # Spectre mitigation info
            /wd4371 # Layout of base class has changed
        )
    endif()
else()
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Please specify either 'Debug' or 'Release' with 'cmake -DCMAKE_BUILD_TYPE=Type'.")
endif()