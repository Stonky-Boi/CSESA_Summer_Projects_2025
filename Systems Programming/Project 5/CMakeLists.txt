cmake_minimum_required(VERSION 3.12)
project(MIPSSimulator VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/mips_simulator.cpp
    src/instruction_decoder.cpp
    src/alu.cpp
    src/pipeline.cpp
    src/branch_predictor.cpp
)

# Header files
set(HEADERS
    include/mips_simulator.hpp
    include/instruction_decoder.hpp
    include/alu.hpp
    include/pipeline.hpp
    include/branch_predictor.hpp
)

# Create library
add_library(mips_simulator_lib ${SOURCES} ${HEADERS})

# Create main executable
add_executable(mips_simulator src/main.cpp)
target_link_libraries(mips_simulator mips_simulator_lib)

# Create CLI interface executable
add_executable(mips_cli src/cli_interface.cpp)
target_link_libraries(mips_cli mips_simulator_lib)

# Installation
install(TARGETS mips_simulator mips_cli
        RUNTIME DESTINATION bin)

install(FILES ${HEADERS}
        DESTINATION include/mips_simulator)

# Testing (optional)
option(BUILD_TESTS "Build test programs" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    doxygen_add_docs(docs ${HEADERS} ${SOURCES})
endif()
