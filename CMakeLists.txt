cmake_minimum_required(VERSION 3.15)
project(MIPS_Simulator VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler-specific options
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    else()
        add_compile_options(-O3)
    endif()
endif()

# Find required packages
find_package(pybind11 REQUIRED)

# Option to build tests
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" ON)
option(BUILD_EXAMPLES "Build example programs" ON)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
)

# Core library source files
set(CORE_SOURCES
    src/core/mips_simulator.cpp
    src/core/instruction.cpp
    src/core/memory.cpp
    src/core/register_file.cpp
    src/core/pipeline.cpp
    src/core/branch_predictor.cpp
    src/core/hazard_detection.cpp
)

# Core library header files
set(CORE_HEADERS
    src/core/mips_simulator.h
    src/core/instruction.h
    src/core/memory.h
    src/core/register_file.h
    src/core/pipeline.h
    src/core/branch_predictor.h
    src/core/hazard_detection.h
)

# Utility source files
set(UTIL_SOURCES
    src/utils/instruction_parser.cpp
    src/utils/disassembler.cpp
)

# Utility header files
set(UTIL_HEADERS
    src/utils/instruction_parser.h
    src/utils/disassembler.h
)

# Create core library
add_library(mips_simulator_lib STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${UTIL_SOURCES}
    ${UTIL_HEADERS}
)

# Set library properties
set_target_properties(mips_simulator_lib PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${CORE_HEADERS};${UTIL_HEADERS}"
)

# Create standalone executable (optional)
add_executable(mips_simulator_cli
    src/main.cpp
)

target_link_libraries(mips_simulator_cli mips_simulator_lib)

# Python bindings
if(BUILD_PYTHON_BINDINGS)
    pybind11_add_module(mips_simulator_core
        python_bindings/pybind_wrapper.cpp
    )
    
    target_link_libraries(mips_simulator_core PRIVATE mips_simulator_lib)
    
    # Compiler-specific options for pybind11
    target_compile_definitions(mips_simulator_core PRIVATE VERSION_INFO=${PROJECT_VERSION})
    
    # Set properties for Python module
    set_target_properties(mips_simulator_core PROPERTIES
        CXX_VISIBILITY_PRESET "hidden"
        VISIBILITY_INLINES_HIDDEN YES
    )
endif()

# Testing
if(BUILD_TESTS)
    enable_testing()
    
    # Find Google Test
    find_package(GTest QUIET)
    if(NOT GTest_FOUND)
        # Download and build Google Test
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9fd7188c5d5b8eb74b21b96.zip
        )
        
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
        
        # Define alias targets for consistency
        add_library(GTest::gtest ALIAS gtest)
        add_library(GTest::gtest_main ALIAS gtest_main)
        add_library(GTest::gmock ALIAS gmock)
        add_library(GTest::gmock_main ALIAS gmock_main)
    endif()
    
    # Test executable
    add_executable(run_tests
        tests/test_mips_simulator.cpp
        tests/test_pipeline.cpp
        tests/test_branch_predictor.cpp
        tests/test_memory.cpp
        tests/test_instruction.cpp
    )
    
    target_link_libraries(run_tests
        mips_simulator_lib
        GTest::gtest
        GTest::gtest_main
    )
    
    # Register tests
    add_test(NAME MIPSSimulatorTests COMMAND run_tests)
    
    # Coverage target (if supported)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
        if(ENABLE_COVERAGE)
            target_compile_options(mips_simulator_lib PRIVATE --coverage)
            target_link_options(mips_simulator_lib PRIVATE --coverage)
            target_compile_options(run_tests PRIVATE --coverage)
            target_link_options(run_tests PRIVATE --coverage)
        endif()
    endif()
endif()

# Example programs
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Documentation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    # Configure Doxyfile
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Installation
include(GNUInstallDirs)

# Install library
install(TARGETS mips_simulator_lib
    EXPORT MIPSSimulatorTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mips_simulator
)

# Install executable
install(TARGETS mips_simulator_cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install Python module (if built)
if(BUILD_PYTHON_BINDINGS)
    install(TARGETS mips_simulator_core
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/python
    )
endif()

# Install headers
install(DIRECTORY src/core/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mips_simulator
    FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY src/utils/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mips_simulator
    FILES_MATCHING PATTERN "*.h"
)

# Create and install CMake config files
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    MIPSSimulatorConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/MIPSSimulatorConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MIPSSimulatorConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MIPSSimulator
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/MIPSSimulatorConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/MIPSSimulatorConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MIPSSimulator
)

install(EXPORT MIPSSimulatorTargets
    FILE MIPSSimulatorTargets.cmake
    NAMESPACE MIPSSimulator::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/MIPSSimulator
)

# Package configuration
set(CPACK_PACKAGE_NAME "MIPS Simulator")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MIPS Processor Simulator with Pipeline and Branch Prediction")
set(CPACK_PACKAGE_VENDOR "MIPS Simulator Team")
set(CPACK_PACKAGE_CONTACT "developer@mipssimulator.org")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

# Platform-specific packaging
if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "MIPS Simulator")
    set(CPACK_NSIS_HELP_LINK "https://github.com/your-repo/mips-simulator")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/your-repo/mips-simulator")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
    set(CPACK_DMG_FORMAT "UDBZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    
    # Debian package configuration
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.17)")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    
    # RPM package configuration
    set(CPACK_RPM_PACKAGE_REQUIRES "glibc >= 2.17")
    set(CPACK_RPM_PACKAGE_GROUP "Development/Tools")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# Summary
message(STATUS "")
message(STATUS "MIPS Simulator Configuration Summary:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  Build Python bindings: ${BUILD_PYTHON_BINDINGS}")
message(STATUS "  Build examples: ${BUILD_EXAMPLES}")
if(ENABLE_COVERAGE)
    message(STATUS "  Coverage reporting: ON")
endif()
if(DOXYGEN_FOUND)
    message(STATUS "  Documentation: Available (use 'make docs')")
endif()
message(STATUS "")