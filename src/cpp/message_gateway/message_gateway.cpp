#include "message_gateway/message_gateway.hpp"
#include "message_gateway/protocol.hpp"

namespace skyguardis {
namespace gateway {

void MessageGateway::sendTargetAssignment(const protocol::TargetAssignment& assignment) {
    // Placeholder: In real implementation, would serialize and send via UDP/IPC
    (void)assignment;  // Suppress unused parameter warning
}

protocol::EngagementStatus MessageGateway::receiveEngagementStatus() {
    // Placeholder: In real implementation, would receive and deserialize from UDP/IPC
    protocol::EngagementStatus status = {};
    return status;
}

} // namespace gateway
} // namespace skyguardis

