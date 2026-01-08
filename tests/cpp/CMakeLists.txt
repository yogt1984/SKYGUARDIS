# C++ test configuration
add_executable(test_threat_evaluator 
    test_threat_evaluator.cpp
    ../../src/cpp/c2_controller/threat_evaluator.cpp
)
target_include_directories(test_threat_evaluator PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
add_test(NAME ThreatEvaluator COMMAND test_threat_evaluator)

add_executable(test_message_gateway
    test_message_gateway.cpp
    ../../src/cpp/message_gateway/protocol.cpp
    ../../src/cpp/message_gateway/message_gateway.cpp
)
target_include_directories(test_message_gateway PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
target_link_libraries(test_message_gateway)
add_test(NAME MessageGateway COMMAND test_message_gateway)

add_executable(test_state_machine_integration
    test_state_machine_integration.cpp
    ../../src/cpp/message_gateway/protocol.cpp
    ../../src/cpp/message_gateway/message_gateway.cpp
)
target_include_directories(test_state_machine_integration PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
target_link_libraries(test_state_machine_integration pthread)
add_test(NAME StateMachineIntegration COMMAND test_state_machine_integration)

add_executable(test_radar_simulation
    test_radar_simulation.cpp
    ../../src/cpp/radar_simulator/radar_simulator.cpp
    ../../src/cpp/radar_simulator/scenario_manager.cpp
)
target_include_directories(test_radar_simulation PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
add_test(NAME RadarSimulation COMMAND test_radar_simulation)

add_executable(test_logging
    test_logging.cpp
    ../../src/cpp/logger/logger.cpp
)
target_include_directories(test_logging PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
add_test(NAME Logging COMMAND test_logging)

add_executable(test_comprehensive_integration
    test_comprehensive_integration.cpp
    ../../src/cpp/message_gateway/protocol.cpp
    ../../src/cpp/message_gateway/message_gateway.cpp
    ../../src/cpp/c2_controller/threat_evaluator.cpp
    ../../src/cpp/radar_simulator/radar_simulator.cpp
    ../../src/cpp/radar_simulator/scenario_manager.cpp
)
target_include_directories(test_comprehensive_integration PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
target_link_libraries(test_comprehensive_integration pthread)
add_test(NAME ComprehensiveIntegration COMMAND test_comprehensive_integration)

add_executable(test_safety_comprehensive
    test_safety_comprehensive.cpp
    ../../src/cpp/c2_controller/threat_evaluator.cpp
)
target_include_directories(test_safety_comprehensive PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
add_test(NAME SafetyComprehensive COMMAND test_safety_comprehensive)

add_executable(test_ballistics_comprehensive
    test_ballistics_comprehensive.cpp
)
target_include_directories(test_ballistics_comprehensive PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
add_test(NAME BallisticsComprehensive COMMAND test_ballistics_comprehensive)

