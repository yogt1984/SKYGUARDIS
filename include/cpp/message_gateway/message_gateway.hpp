#pragma once

#include "message_gateway/protocol.hpp"

namespace skyguardis {
namespace gateway {

class MessageGateway {
public:
    void sendTargetAssignment(const protocol::TargetAssignment& assignment);
    protocol::EngagementStatus receiveEngagementStatus();

private:
    // UDP/IPC implementation
};

} // namespace gateway
} // namespace skyguardis

