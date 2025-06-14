cmake_minimum_required(VERSION 3.12)
project(MIPS_Simulator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(include)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

# Create main executable
add_executable(mips_simulator ${SOURCES} ${HEADERS})

# Create library for Python bindings (optional)
add_library(mips_simulator_lib SHARED ${SOURCES})
set_target_properties(mips_simulator_lib PROPERTIES
    OUTPUT_NAME "mips_simulator"
    PREFIX ""
)

# Platform-specific settings
if(WIN32)
    set_target_properties(mips_simulator_lib PROPERTIES SUFFIX ".dll")
elseif(APPLE)
    set_target_properties(mips_simulator_lib PROPERTIES SUFFIX ".dylib")
else()
    set_target_properties(mips_simulator_lib PROPERTIES SUFFIX ".so")
endif()

# Find and link required libraries
find_package(Threads REQUIRED)
target_link_libraries(mips_simulator Threads::Threads)
target_link_libraries(mips_simulator_lib Threads::Threads)

# Optional: Find Python for bindings
find_package(Python3 COMPONENTS Interpreter Development)
if(Python3_FOUND)
    message(STATUS "Python3 found: ${Python3_EXECUTABLE}")
    message(STATUS "Python3 libraries: ${Python3_LIBRARIES}")

    # Python binding target
    add_library(pymips SHARED ${SOURCES})
    target_include_directories(pymips PRIVATE ${Python3_INCLUDE_DIRS})
    target_link_libraries(pymips ${Python3_LIBRARIES})
    set_target_properties(pymips PROPERTIES
        PREFIX ""
        SUFFIX ".so"
        OUTPUT_NAME "pymips"
    )
endif()

# Installation rules
install(TARGETS mips_simulator
        RUNTIME DESTINATION bin)

install(TARGETS mips_simulator_lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# Install headers
install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")

# Create package config
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/MIPS_SimulatorConfigVersion.cmake"
    VERSION 1.0.0
    COMPATIBILITY AnyNewerVersion
)

# Testing (optional)
option(BUILD_TESTS "Build test programs" OFF)
if(BUILD_TESTS)
    enable_testing()

    # Add test executable
    add_executable(mips_simulator_test test/test_main.cpp ${SOURCES})
    target_include_directories(mips_simulator_test PRIVATE include test)
    target_link_libraries(mips_simulator_test Threads::Threads)

    add_test(NAME BasicTests COMMAND mips_simulator_test)
endif()

# Documentation (optional)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_PROJECT_NAME "MIPS Simulator")
    set(DOXYGEN_PROJECT_BRIEF "Software-based MIPS instruction set simulator")

    doxygen_add_docs(docs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMENT "Generating documentation"
    )
endif()

# Print configuration summary
message(STATUS "Build configuration:")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  Version: 1.0.0")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}")