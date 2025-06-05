cmake_minimum_required(VERSION 3.16)

# Project name and language standard
project(kvs++ VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use folders in the IDE (optional, helps organization in Visual Studio / CLion)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include all header directories
include_directories(include)

# Collect library source files (excluding main files)
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
    src/persistence/*.cpp
    src/utils/*.cpp
    src/cli/*.cpp
)

# Create a static library for the core functionality
add_library(kvstore STATIC ${LIB_SOURCES})

# Executable 1: Test suite entry point
add_executable(kvspp-test src/tests_main.cpp)
target_link_libraries(kvspp-test kvstore)

# Executable 2: Single command CLI 
add_executable(kvspp-single-cmd src/single_cmd_main.cpp)
target_link_libraries(kvspp-single-cmd kvstore)

# Executable 3: Interactive CLI
add_executable(kvspp-cli src/cli_main.cpp)
target_link_libraries(kvspp-cli kvstore)

# Executable 4: Demo application
add_executable(kvspp-demo src/demo_main.cpp)
target_link_libraries(kvspp-demo kvstore)

# Set up linking for all executables
set(ALL_TARGETS kvspp-test kvspp-single-cmd kvspp-cli kvspp-demo)

foreach(target ${ALL_TARGETS})
    # make sure MinGW doesn't complain about missing entry points
    if (MINGW)
        target_link_libraries(${target} stdc++fs)
    endif()

    # Link with pthread if needed (for thread safety in the store)
    if (UNIX OR MINGW)
        find_package(Threads REQUIRED)
        target_link_libraries(${target} Threads::Threads)
    endif()

    # Set compile warnings
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endforeach()
