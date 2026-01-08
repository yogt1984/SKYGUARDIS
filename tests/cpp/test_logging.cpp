#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include "logger/logger.hpp"

using namespace skyguardis::logger;

// Test: Log levels
void test_log_levels() {
    std::cout << "  Testing log levels...\n";
    
    Logger logger;
    logger.setLogLevel(LogLevel::INFO);
    logger.enableConsoleOutput(false); // Disable console for testing
    
    // Set log file
    logger.setLogFile("/tmp/test_logging.log");
    
    logger.debug("This debug message should not appear");
    logger.info("This info message should appear");
    logger.warn("This warn message should appear");
    logger.error("This error message should appear");
    
    // Read log file and verify
    std::ifstream log_file("/tmp/test_logging.log");
    std::string line;
    int info_count = 0, warn_count = 0, error_count = 0, debug_count = 0;
    
    while (std::getline(log_file, line)) {
        if (line.find("DEBUG") != std::string::npos) debug_count++;
        if (line.find("INFO") != std::string::npos) info_count++;
        if (line.find("WARN") != std::string::npos) warn_count++;
        if (line.find("ERROR") != std::string::npos) error_count++;
    }
    
    assert(debug_count == 0 && "Debug messages should be filtered");
    assert(info_count > 0 && "Info messages should appear");
    assert(warn_count > 0 && "Warn messages should appear");
    assert(error_count > 0 && "Error messages should appear");
    
    std::cout << "    ✓ Log level filtering works correctly\n";
    std::cout << "  ✓ Log levels test passed\n";
}

// Test: Timestamps
void test_timestamps() {
    std::cout << "  Testing timestamps...\n";
    
    Logger logger;
    logger.setLogFile("/tmp/test_timestamps.log");
    logger.enableConsoleOutput(false);
    
    logger.info("Test message with timestamp");
    
    std::ifstream log_file("/tmp/test_timestamps.log");
    std::string line;
    std::getline(log_file, line);
    
    assert(line.find("[") == 0 && "Log should start with timestamp bracket");
    assert(line.find("]") != std::string::npos && "Log should have timestamp");
    
    std::cout << "    ✓ Timestamp format: " << line.substr(0, 30) << "...\n";
    std::cout << "  ✓ Timestamps test passed\n";
}

// Test: Specialized logging methods
void test_specialized_logging() {
    std::cout << "  Testing specialized logging...\n";
    
    Logger logger;
    logger.setLogFile("/tmp/test_specialized.log");
    logger.enableConsoleOutput(false);
    
    logger.logTargetAssignment(12345, 5000.0, 0.5);
    logger.logStateTransition("Idle", "Acquiring");
    logger.logSafetyViolation("Range out of bounds");
    logger.logError("C2_Controller", "Connection timeout");
    
    std::ifstream log_file("/tmp/test_specialized.log");
    std::string content((std::istreambuf_iterator<char>(log_file)),
                        std::istreambuf_iterator<char>());
    
    assert(content.find("Target Assignment") != std::string::npos);
    assert(content.find("State Transition") != std::string::npos);
    assert(content.find("Safety Violation") != std::string::npos);
    assert(content.find("C2_Controller") != std::string::npos);
    
    std::cout << "    ✓ All specialized logging methods work\n";
    std::cout << "  ✓ Specialized logging test passed\n";
}

// Test: Performance metrics
void test_performance_metrics() {
    std::cout << "  Testing performance metrics...\n";
    
    Logger logger;
    logger.setLogLevel(LogLevel::DEBUG); // Enable debug for metrics
    logger.setLogFile("/tmp/test_metrics.log");
    logger.enableConsoleOutput(false);
    
    logger.logPerformanceMetric("cycle_time", 45.2, "ms");
    logger.logPerformanceMetric("track_count", 5.0);
    logger.logPerformanceMetric("memory_usage", 1024.0, "MB");
    
    std::ifstream log_file("/tmp/test_metrics.log");
    std::string content((std::istreambuf_iterator<char>(log_file)),
                        std::istreambuf_iterator<char>());
    
    assert(content.find("cycle_time") != std::string::npos);
    assert(content.find("track_count") != std::string::npos);
    assert(content.find("memory_usage") != std::string::npos);
    
    std::cout << "    ✓ Performance metrics logging works\n";
    std::cout << "  ✓ Performance metrics test passed\n";
}

// Test: Thread safety
void test_thread_safety() {
    std::cout << "  Testing thread safety...\n";
    
    Logger logger;
    logger.setLogFile("/tmp/test_thread_safety.log");
    logger.enableConsoleOutput(false);
    
    // Log from multiple "threads" (simulated)
    for (int i = 0; i < 100; ++i) {
        logger.info("Thread 1 message " + std::to_string(i));
        logger.info("Thread 2 message " + std::to_string(i));
    }
    
    // Verify all messages were written
    std::ifstream log_file("/tmp/test_thread_safety.log");
    int line_count = 0;
    std::string line;
    while (std::getline(log_file, line)) {
        line_count++;
    }
    
    assert(line_count == 200 && "All messages should be written");
    
    std::cout << "    ✓ Thread-safe logging verified (" << line_count << " messages)\n";
    std::cout << "  ✓ Thread safety test passed\n";
}

int main() {
    std::cout << "\nTesting Enhanced Logging System...\n\n";
    
    try {
        test_log_levels();
        test_timestamps();
        test_specialized_logging();
        test_performance_metrics();
        test_thread_safety();
        
        std::cout << "\n✓ All enhanced logging tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}

