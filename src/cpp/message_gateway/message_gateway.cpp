#include "message_gateway/message_gateway.hpp"
#include "message_gateway/protocol.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

namespace skyguardis {
namespace gateway {

MessageGateway::MessageGateway() 
    : send_socket_(-1), receive_socket_(-1), gun_control_addr_(nullptr), initialized_(false) {
    gun_control_addr_ = new struct sockaddr_in;
    std::memset(gun_control_addr_, 0, sizeof(struct sockaddr_in));
}

MessageGateway::~MessageGateway() {
    shutdown();
    if (gun_control_addr_) {
        delete gun_control_addr_;
        gun_control_addr_ = nullptr;
    }
}

bool MessageGateway::initialize(uint16_t gun_control_port, uint16_t c2_receive_port) {
    if (initialized_) {
        return true;
    }
    
    // Create send socket
    send_socket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (send_socket_ < 0) {
        return false;
    }
    
    // Setup gun control address
    gun_control_addr_->sin_family = AF_INET;
    gun_control_addr_->sin_addr.s_addr = inet_addr("127.0.0.1");
    gun_control_addr_->sin_port = htons(gun_control_port);
    
    // Create receive socket
    receive_socket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (receive_socket_ < 0) {
        close(send_socket_);
        send_socket_ = -1;
        return false;
    }
    
    // Set socket to non-blocking
    int flags = fcntl(receive_socket_, F_GETFL, 0);
    fcntl(receive_socket_, F_SETFL, flags | O_NONBLOCK);
    
    // Bind receive socket
    struct sockaddr_in receive_addr;
    std::memset(&receive_addr, 0, sizeof(receive_addr));
    receive_addr.sin_family = AF_INET;
    receive_addr.sin_addr.s_addr = INADDR_ANY;
    receive_addr.sin_port = htons(c2_receive_port);
    
    if (bind(receive_socket_, (struct sockaddr*)&receive_addr, sizeof(receive_addr)) < 0) {
        close(send_socket_);
        close(receive_socket_);
        send_socket_ = -1;
        receive_socket_ = -1;
        return false;
    }
    
    initialized_ = true;
    return true;
}

bool MessageGateway::sendTargetAssignment(const protocol::TargetAssignment& assignment) {
    if (!initialized_) {
        return false;
    }
    
    uint8_t buffer[protocol::TargetAssignment::SERIALIZED_SIZE];
    if (!protocol::serializeTargetAssignment(assignment, buffer, sizeof(buffer))) {
        return false;
    }
    
    ssize_t sent = sendto(send_socket_, buffer, sizeof(buffer), 0,
                         (struct sockaddr*)gun_control_addr_, sizeof(struct sockaddr_in));
    
    return sent == static_cast<ssize_t>(sizeof(buffer));
}

bool MessageGateway::receiveEngagementStatus(protocol::EngagementStatus& status) {
    if (!initialized_) {
        return false;
    }
    
    uint8_t buffer[protocol::EngagementStatus::SERIALIZED_SIZE];
    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);
    
    ssize_t received = recvfrom(receive_socket_, buffer, sizeof(buffer), 0,
                                (struct sockaddr*)&from_addr, &from_len);
    
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available (non-blocking)
            return false;
        }
        return false;
    }
    
    if (received != static_cast<ssize_t>(sizeof(buffer))) {
        return false;
    }
    
    return protocol::deserializeEngagementStatus(buffer, received, status);
}

void MessageGateway::shutdown() {
    if (send_socket_ >= 0) {
        close(send_socket_);
        send_socket_ = -1;
    }
    if (receive_socket_ >= 0) {
        close(receive_socket_);
        receive_socket_ = -1;
    }
    initialized_ = false;
}

} // namespace gateway
} // namespace skyguardis
