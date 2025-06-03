cmake_minimum_required(VERSION 3.16)

# Project name and language standard
project(kvc++ VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use folders in the IDE (optional, helps organization in Visual Studio / CLion)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include all header directories
include_directories(include)

# Collect all source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# Add the executable
add_executable(kvc++ ${SOURCES})

# make sure MinGW doesn't complain about missing entry points
if (MINGW)
    target_link_libraries(kvc++ stdc++fs)
endif()

# Link with pthread if needed (for thread safety in the store)
if (UNIX OR MINGW)
    find_package(Threads REQUIRED)
    target_link_libraries(kvc++ Threads::Threads)
endif()

# Set compile warnings
if (MSVC)
    target_compile_options(kvc++ PRIVATE /W4 /WX)
else()
    target_compile_options(kvc++ PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
