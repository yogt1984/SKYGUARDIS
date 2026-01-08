#pragma once

#include "message_gateway/protocol.hpp"
#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <chrono>

namespace skyguardis {
namespace logger {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3
};

class Logger {
public:
    Logger();
    ~Logger();
    
    // Logging methods with levels
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    
    // Legacy method (maps to INFO)
    void log(const std::string& message);
    
    // Specialized logging
    void logEngagement(const protocol::EngagementStatus& status);
    void logTargetAssignment(uint32_t target_id, double range_m, double azimuth_rad);
    void logStateTransition(const std::string& from_state, const std::string& to_state);
    void logSafetyViolation(const std::string& reason);
    void logError(const std::string& component, const std::string& error);
    
    // Configuration
    void setLogLevel(LogLevel level);
    void setLogFile(const std::string& filename);
    void enableConsoleOutput(bool enable);
    
    // Performance metrics
    void logPerformanceMetric(const std::string& metric_name, double value, const std::string& unit = "");

private:
    LogLevel current_level_;
    std::unique_ptr<std::ofstream> log_file_;
    std::mutex log_mutex_;
    bool console_output_;
    
    std::string getTimestamp() const;
    std::string levelToString(LogLevel level) const;
    void writeLog(LogLevel level, const std::string& message);
};

} // namespace logger
} // namespace skyguardis
