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
    "${CMAKE_CURRENT_SOURCE_DIR}/VarjusAPI/include/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/fs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/linter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/runtime/*.cpp"
)

# Filter out CMake's compiler detection files
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp$")

add_executable(Varjus ${PROJECT_SOURCES})

if(USE_UNICODE STREQUAL "ON")
    add_compile_definitions(UNICODE=1 _UNICODE=1 UNICODE)
    message(STATUS "Building with Unicode")
else()
    remove_definitions(UNICODE _UNICODE)
    message(STATUS "Building with UTF8")
endif()


# Add the root directory as an include directory
target_include_directories(Varjus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(Varjus PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/VarjusAPI/include/")

set_target_properties(Varjus PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

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

    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG_MODE=1")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Werror -pedantic -O0 -g")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /WX /external:W3 /Od /D_DEBUG /ZI /RTC1")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4514 /wd4626 /wd4820 /wd5267 /wd4351 /wd5045 /wd4371 /wd4324")
        target_compile_definitions(Varjus PRIVATE /DEBUG)
    endif()


else()
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Please specify either 'Debug' or 'Release' with 'cmake -DCMAKE_BUILD_TYPE=Type'.")
endif()