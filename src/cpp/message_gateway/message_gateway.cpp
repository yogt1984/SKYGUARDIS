#include "message_gateway/protocol.hpp"
#include "message_gateway/message_gateway.hpp"

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

