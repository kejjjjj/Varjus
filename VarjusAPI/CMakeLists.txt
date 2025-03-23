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

# Collect all header files
file(GLOB_RECURSE API_HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# Exclude files inside the build directory
list(FILTER API_HEADERS EXCLUDE REGEX "${CMAKE_BINARY_DIR}/.*")
list(FILTER API_HEADERS EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/VarjusAPI/.*")

# Filter out CMake's compiler detection files
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp$")

# Create a static library
add_library(VarjusAPI STATIC ${API_SOURCES})

if(USE_UNICODE STREQUAL "ON")
    add_compile_definitions(UNICODE=1 _UNICODE=1 UNICODE)
else()
    remove_definitions(UNICODE _UNICODE)
endif()

# Set include directories
target_include_directories(VarjusAPI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories(VarjusAPI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

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
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /WX /external:W3 /Od /D_DEBUG /ZI /RTC1")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4514 /wd4626 /wd4820 /wd5267 /wd4351 /wd5045 /wd4371 /wd4324")
        target_compile_definitions(Varjus PRIVATE /DEBUG)
    endif()

else()
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}. Please specify either 'Debug' or 'Release' with 'cmake -DCMAKE_BUILD_TYPE=Type'.")
endif()

# Define output directory
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/VarjusAPI")

# Copy the static library
add_custom_command(TARGET VarjusAPI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIR}/lib"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:VarjusAPI>" "${OUTPUT_DIR}/lib/"
)

# Copy header files while maintaining relative structure
foreach(header ${API_HEADERS})
    file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}/.." "${header}")
    get_filename_component(dir_path "${rel_path}" DIRECTORY)
    
    add_custom_command(TARGET VarjusAPI POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/${dir_path}"
        COMMAND ${CMAKE_COMMAND} -E copy "${header}" "${CMAKE_CURRENT_SOURCE_DIR}/${rel_path}"
    )
endforeach()