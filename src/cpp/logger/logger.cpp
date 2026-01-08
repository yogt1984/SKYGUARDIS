#include "logger/logger.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace skyguardis {
namespace logger {

Logger::Logger() : current_level_(LogLevel::INFO), console_output_(true) {}

Logger::~Logger() {
    if (log_file_ && log_file_->is_open()) {
        log_file_->close();
    }
}

std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO ";
        case LogLevel::WARN:  return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

void Logger::writeLog(LogLevel level, const std::string& message) {
    if (level < current_level_) {
        return; // Filter by log level
    }
    
    std::lock_guard<std::mutex> lock(log_mutex_);
    
    std::string timestamp = getTimestamp();
    std::string level_str = levelToString(level);
    std::string log_entry = "[" + timestamp + "] [" + level_str + "] " + message;
    
    // Console output
    if (console_output_) {
        std::cout << log_entry << std::endl;
    }
    
    // File output
    if (log_file_ && log_file_->is_open()) {
        *log_file_ << log_entry << std::endl;
        log_file_->flush();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    writeLog(level, message);
}

void Logger::debug(const std::string& message) {
    writeLog(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    writeLog(LogLevel::INFO, message);
}

void Logger::warn(const std::string& message) {
    writeLog(LogLevel::WARN, message);
}

void Logger::error(const std::string& message) {
    writeLog(LogLevel::ERROR, message);
}

void Logger::log(const std::string& message) {
    writeLog(LogLevel::INFO, message);
}

void Logger::logEngagement(const protocol::EngagementStatus& status) {
    std::stringstream ss;
    ss << "Engagement Status: target_id=" << status.target_id
       << " state=" << static_cast<int>(status.state)
       << " firing=" << static_cast<int>(status.firing)
       << " lead_angle=" << status.lead_angle_rad << "rad"
       << " time_to_impact=" << status.time_to_impact_s << "s";
    writeLog(LogLevel::INFO, ss.str());
}

void Logger::logTargetAssignment(uint32_t target_id, double range_m, double azimuth_rad) {
    std::stringstream ss;
    ss << "Target Assignment: id=" << target_id
       << " range=" << range_m << "m"
       << " azimuth=" << azimuth_rad << "rad";
    writeLog(LogLevel::INFO, ss.str());
}

void Logger::logStateTransition(const std::string& from_state, const std::string& to_state) {
    std::stringstream ss;
    ss << "State Transition: " << from_state << " -> " << to_state;
    writeLog(LogLevel::INFO, ss.str());
}

void Logger::logSafetyViolation(const std::string& reason) {
    std::stringstream ss;
    ss << "Safety Violation: " << reason;
    writeLog(LogLevel::WARN, ss.str());
}

void Logger::logError(const std::string& component, const std::string& error) {
    std::stringstream ss;
    ss << "[" << component << "] " << error;
    writeLog(LogLevel::ERROR, ss.str());
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    current_level_ = level;
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    if (log_file_ && log_file_->is_open()) {
        log_file_->close();
    }
    log_file_ = std::make_unique<std::ofstream>(filename, std::ios::app);
    if (!log_file_->is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    console_output_ = enable;
}

void Logger::logPerformanceMetric(const std::string& metric_name, double value, const std::string& unit) {
    std::stringstream ss;
    ss << "Performance [" << metric_name << "]: " << value;
    if (!unit.empty()) {
        ss << " " << unit;
    }
    writeLog(LogLevel::DEBUG, ss.str());
}

} // namespace logger
} // namespace skyguardis
