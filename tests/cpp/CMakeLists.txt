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

