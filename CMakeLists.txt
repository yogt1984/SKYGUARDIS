cmake_minimum_required(VERSION 3.16)
project(SKYGUARDIS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include/cpp)

# C++ source files
set(C2_SOURCES
    src/cpp/c2_controller/c2_controller.cpp
    src/cpp/c2_controller/threat_evaluator.cpp
)

set(RADAR_SOURCES
    src/cpp/radar_simulator/radar_simulator.cpp
)

set(MESSAGE_GATEWAY_SOURCES
    src/cpp/message_gateway/message_gateway.cpp
    src/cpp/message_gateway/protocol.cpp
)

set(LOGGER_SOURCES
    src/cpp/logger/logger.cpp
    src/cpp/logger/visualizer.cpp
)

# Executables
add_executable(c2_node 
    src/cpp/main_c2_node.cpp
    ${C2_SOURCES} 
    ${MESSAGE_GATEWAY_SOURCES}
)
add_executable(radar_sim 
    src/cpp/main_radar_sim.cpp
    ${RADAR_SOURCES}
)
add_executable(logger ${LOGGER_SOURCES})

# Link libraries
target_link_libraries(c2_node)
target_link_libraries(radar_sim)
target_link_libraries(logger)

# Tests
enable_testing()
add_subdirectory(tests/cpp)

