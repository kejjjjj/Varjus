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
file(GLOB_RECURSE PROJECT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../VarjusAPI/include/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../fs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../linter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../runtime/*.cpp"
)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp$")

add_executable(Varjus ${PROJECT_SOURCES})

if(USE_UNICODE STREQUAL "ON")
    add_compile_definitions(UNICODE=1 _UNICODE=1 UNICODE)
else()
    remove_definitions(UNICODE _UNICODE)
endif()

# Add the root directory as an include directory
target_include_directories(Varjus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../")
target_include_directories(Varjus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../VarjusAPI/include/")
target_include_directories(Varjus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../VarjusAPI/include/varjus_api/types/std/json")

# define RUNNING_TESTS
target_compile_definitions(Varjus PRIVATE RUNNING_TESTS)

add_custom_command(TARGET Varjus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:Varjus> "${CMAKE_CURRENT_SOURCE_DIR}/")


message(STATUS "Building tests...")

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building in Release mode")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Werror -pedantic -O3")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /WX /O2 /MT")
	    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4514 /wd4626 /wd4820 /wd5267 /wd4351 /wd5045 /wd4371 /wd4324")
    endif()

elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror -pedantic -O0 -g")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /WX /external:W3 /Od /D_DEBUG /RTC1")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4514 /wd4626 /wd4820 /wd5267 /wd4351 /wd5045 /wd4371 /wd4324")
        target_compile_definitions(Varjus PRIVATE /DEBUG)
    endif()

else()
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Please specify either 'Debug' or 'Release' with 'cmake -DCMAKE_BUILD_TYPE=Type'.")
    return()
endif()
