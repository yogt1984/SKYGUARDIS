# C++ test configuration
add_executable(test_threat_evaluator 
    test_threat_evaluator.cpp
    ../../src/cpp/c2_controller/threat_evaluator.cpp
)
target_include_directories(test_threat_evaluator PRIVATE 
    ${CMAKE_SOURCE_DIR}/include/cpp
)
add_test(NAME ThreatEvaluator COMMAND test_threat_evaluator)

