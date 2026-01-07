#pragma once

#include "message_gateway/protocol.hpp"
#include <cstdint>

// Forward declarations
struct sockaddr_in;

namespace skyguardis {
namespace gateway {

class MessageGateway {
public:
    MessageGateway();
    ~MessageGateway();
    
    // Initialize UDP sockets
    bool initialize(uint16_t gun_control_port = 8888, uint16_t c2_receive_port = 8889);
    
    // Send target assignment to gun control
    bool sendTargetAssignment(const protocol::TargetAssignment& assignment);
    
    // Receive engagement status from gun control (non-blocking)
    bool receiveEngagementStatus(protocol::EngagementStatus& status);
    
    // Cleanup
    void shutdown();
    
    bool isInitialized() const { return initialized_; }

private:
    int send_socket_;
    int receive_socket_;
    struct sockaddr_in* gun_control_addr_;
    bool initialized_;
    
    static constexpr int SOCKET_TIMEOUT_MS = 100;
};

} // namespace gateway
} // namespace skyguardis

