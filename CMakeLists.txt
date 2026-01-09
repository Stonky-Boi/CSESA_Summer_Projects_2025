cmake_minimum_required(VERSION 3.10)
project(MipsSimulator)

# Require C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directory for the executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/web)

# Include the header files directory
include_directories(include)

# Find all source files in the src directory
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(mips_simulator ${SOURCES})

# Add a custom command to inform the user where the executable is
add_custom_target(info ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Executable will be placed in ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)