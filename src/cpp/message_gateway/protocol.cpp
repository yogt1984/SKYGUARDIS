#include "message_gateway/protocol.hpp"
#include <cstring>
#include <arpa/inet.h>

namespace skyguardis {
namespace protocol {

// Calculate 16-bit checksum
uint16_t calculateChecksum(const uint8_t* data, size_t length) {
    uint32_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        sum += data[i];
    }
    return static_cast<uint16_t>(sum & 0xFFFF);
}

bool validateChecksum(const uint8_t* data, size_t length, uint16_t checksum) {
    return calculateChecksum(data, length) == checksum;
}

// Serialize TargetAssignment message
bool serializeTargetAssignment(const TargetAssignment& msg, uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < TargetAssignment::SERIALIZED_SIZE) {
        return false;
    }
    
    size_t offset = 0;
    
    // Header
    buffer[offset++] = static_cast<uint8_t>(MessageType::TARGET_ASSIGNMENT);
    buffer[offset++] = 0x01; // Version
    uint16_t length = htons(37); // Payload size
    std::memcpy(buffer + offset, &length, 2);
    offset += 2;
    
    // Placeholder for checksum (will be calculated after)
    uint16_t checksum_pos = offset;
    offset += 2;
    
    // Payload
    uint32_t target_id_net = htonl(msg.target_id);
    std::memcpy(buffer + offset, &target_id_net, 4);
    offset += 4;
    
    // Doubles need to be converted (assuming same endianness for simplicity)
    // In production, would use proper network byte order conversion
    std::memcpy(buffer + offset, &msg.range_m, 8);
    offset += 8;
    std::memcpy(buffer + offset, &msg.azimuth_rad, 8);
    offset += 8;
    std::memcpy(buffer + offset, &msg.elevation_rad, 8);
    offset += 8;
    std::memcpy(buffer + offset, &msg.velocity_ms, 8);
    offset += 8;
    
    buffer[offset++] = msg.priority;
    
    // Calculate and write checksum (excluding checksum field itself)
    uint16_t checksum = calculateChecksum(buffer, checksum_pos);
    checksum += calculateChecksum(buffer + checksum_pos + 2, offset - checksum_pos - 2);
    checksum &= 0xFFFF;
    uint16_t checksum_net = htons(checksum);
    std::memcpy(buffer + checksum_pos, &checksum_net, 2);
    
    return true;
}

// Deserialize TargetAssignment message
bool deserializeTargetAssignment(const uint8_t* buffer, size_t buffer_size, TargetAssignment& msg) {
    if (buffer_size < TargetAssignment::SERIALIZED_SIZE) {
        return false;
    }
    
    // Validate message type
    if (buffer[0] != static_cast<uint8_t>(MessageType::TARGET_ASSIGNMENT)) {
        return false;
    }
    
    // Validate version
    if (buffer[1] != 0x01) {
        return false;
    }
    
    // Extract checksum
    uint16_t received_checksum;
    std::memcpy(&received_checksum, buffer + 4, 2);
    received_checksum = ntohs(received_checksum);
    
    // Validate checksum (excluding checksum field)
    uint16_t calculated_checksum = calculateChecksum(buffer, 4);
    calculated_checksum += calculateChecksum(buffer + 6, TargetAssignment::SERIALIZED_SIZE - 6);
    calculated_checksum &= 0xFFFF;
    
    if (calculated_checksum != received_checksum) {
        return false;
    }
    
    size_t offset = 6;
    
    // Deserialize payload
    uint32_t target_id_net;
    std::memcpy(&target_id_net, buffer + offset, 4);
    msg.target_id = ntohl(target_id_net);
    offset += 4;
    
    std::memcpy(&msg.range_m, buffer + offset, 8);
    offset += 8;
    std::memcpy(&msg.azimuth_rad, buffer + offset, 8);
    offset += 8;
    std::memcpy(&msg.elevation_rad, buffer + offset, 8);
    offset += 8;
    std::memcpy(&msg.velocity_ms, buffer + offset, 8);
    offset += 8;
    
    msg.priority = buffer[offset];
    
    return true;
}

// Serialize EngagementStatus message
bool serializeEngagementStatus(const EngagementStatus& msg, uint8_t* buffer, size_t buffer_size) {
    if (buffer_size < EngagementStatus::SERIALIZED_SIZE) {
        return false;
    }
    
    size_t offset = 0;
    
    // Header
    buffer[offset++] = static_cast<uint8_t>(MessageType::ENGAGEMENT_STATUS);
    buffer[offset++] = 0x01; // Version
    uint16_t length = htons(22); // Payload size
    std::memcpy(buffer + offset, &length, 2);
    offset += 2;
    
    // Placeholder for checksum
    uint16_t checksum_pos = offset;
    offset += 2;
    
    // Payload
    uint32_t target_id_net = htonl(msg.target_id);
    std::memcpy(buffer + offset, &target_id_net, 4);
    offset += 4;
    
    buffer[offset++] = msg.state;
    buffer[offset++] = msg.firing;
    
    std::memcpy(buffer + offset, &msg.lead_angle_rad, 8);
    offset += 8;
    std::memcpy(buffer + offset, &msg.time_to_impact_s, 8);
    offset += 8;
    
    // Calculate and write checksum
    uint16_t checksum = calculateChecksum(buffer, checksum_pos);
    checksum += calculateChecksum(buffer + checksum_pos + 2, offset - checksum_pos - 2);
    checksum &= 0xFFFF;
    uint16_t checksum_net = htons(checksum);
    std::memcpy(buffer + checksum_pos, &checksum_net, 2);
    
    return true;
}

// Deserialize EngagementStatus message
bool deserializeEngagementStatus(const uint8_t* buffer, size_t buffer_size, EngagementStatus& msg) {
    if (buffer_size < EngagementStatus::SERIALIZED_SIZE) {
        return false;
    }
    
    // Validate message type
    if (buffer[0] != static_cast<uint8_t>(MessageType::ENGAGEMENT_STATUS)) {
        return false;
    }
    
    // Validate version
    if (buffer[1] != 0x01) {
        return false;
    }
    
    // Extract checksum
    uint16_t received_checksum;
    std::memcpy(&received_checksum, buffer + 4, 2);
    received_checksum = ntohs(received_checksum);
    
    // Validate checksum
    uint16_t calculated_checksum = calculateChecksum(buffer, 4);
    calculated_checksum += calculateChecksum(buffer + 6, EngagementStatus::SERIALIZED_SIZE - 6);
    calculated_checksum &= 0xFFFF;
    
    if (calculated_checksum != received_checksum) {
        return false;
    }
    
    size_t offset = 6;
    
    // Deserialize payload
    uint32_t target_id_net;
    std::memcpy(&target_id_net, buffer + offset, 4);
    msg.target_id = ntohl(target_id_net);
    offset += 4;
    
    msg.state = buffer[offset++];
    msg.firing = buffer[offset++];
    
    std::memcpy(&msg.lead_angle_rad, buffer + offset, 8);
    offset += 8;
    std::memcpy(&msg.time_to_impact_s, buffer + offset, 8);
    
    return true;
}

} // namespace protocol
} // namespace skyguardis
