#include "logger/logger.hpp"
#include <iostream>
#include <fstream>

namespace skyguardis {
namespace logger {

void Logger::log(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

void Logger::logEngagement(const protocol::EngagementStatus& status) {
    log("Engagement status: target_id=" + std::to_string(status.target_id));
}

} // namespace logger
} // namespace skyguardis

