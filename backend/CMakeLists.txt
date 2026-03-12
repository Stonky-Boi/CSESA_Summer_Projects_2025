cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ember VERSION 1.0)

# Enforce C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tell CMake where to find our header files
include_directories(include)

# Recursively find all .cpp files in the src/ directory
file(GLOB_RECURSE EMBER_SOURCES "src/*.cpp")

# Create the executable
add_executable(ember ${EMBER_SOURCES})