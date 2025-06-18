cmake_minimum_required(VERSION 3.12)
project(MIPSSimulator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

include_directories(include)

file(GLOB SIM_SOURCES src/*.cpp)
file(GLOB SIM_HEADERS include/*.hpp)

add_library(mips_simulator_lib ${SIM_SOURCES} ${SIM_HEADERS})
add_executable(mips_simulator src/main.cpp)
target_link_libraries(mips_simulator mips_simulator_lib)
add_executable(mips_cli src/cli_interface.cpp)
target_link_libraries(mips_cli mips_simulator_lib)

install(TARGETS mips_simulator mips_cli RUNTIME DESTINATION bin)
install(FILES ${SIM_HEADERS} DESTINATION include/mips_simulator)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
  doxygen_add_docs(docs ${SIM_HEADERS} ${SIM_SOURCES})
endif()
