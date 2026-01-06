#pragma once

#include "message_gateway/protocol.hpp"
#include <string>

namespace skyguardis {
namespace logger {

class Logger {
public:
    void log(const std::string& message);
    void logEngagement(const protocol::EngagementStatus& status);
};

} // namespace logger
} // namespace skyguardis

