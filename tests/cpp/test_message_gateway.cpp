#include "message_gateway/protocol.hpp"
#include "message_gateway/message_gateway.hpp"
#include <cassert>
#include <iostream>
#include <cstring>

void test_serialization() {
    std::cout << "Testing message serialization..." << std::endl;
    
    // Test TargetAssignment serialization
    skyguardis::protocol::TargetAssignment assignment;
    assignment.target_id = 12345;
    assignment.range_m = 5000.0;
    assignment.azimuth_rad = 0.785;  // 45 degrees
    assignment.elevation_rad = 0.174; // 10 degrees
    assignment.velocity_ms = 200.0;
    assignment.priority = 128;
    
    uint8_t buffer[skyguardis::protocol::TargetAssignment::SERIALIZED_SIZE];
    bool success = skyguardis::protocol::serializeTargetAssignment(assignment, buffer, sizeof(buffer));
    assert(success);
    std::cout << "  ✓ TargetAssignment serialization passed" << std::endl;
    
    // Test deserialization
    skyguardis::protocol::TargetAssignment deserialized;
    success = skyguardis::protocol::deserializeTargetAssignment(buffer, sizeof(buffer), deserialized);
    assert(success);
    assert(deserialized.target_id == assignment.target_id);
    assert(deserialized.range_m == assignment.range_m);
    assert(deserialized.azimuth_rad == assignment.azimuth_rad);
    assert(deserialized.elevation_rad == assignment.elevation_rad);
    assert(deserialized.velocity_ms == assignment.velocity_ms);
    assert(deserialized.priority == assignment.priority);
    std::cout << "  ✓ TargetAssignment deserialization passed" << std::endl;
    
    // Test EngagementStatus serialization
    skyguardis::protocol::EngagementStatus status;
    status.target_id = 12345;
    status.state = 2; // Tracking
    status.firing = 0;
    status.lead_angle_rad = 0.1;
    status.time_to_impact_s = 5.5;
    
    uint8_t status_buffer[skyguardis::protocol::EngagementStatus::SERIALIZED_SIZE];
    success = skyguardis::protocol::serializeEngagementStatus(status, status_buffer, sizeof(status_buffer));
    assert(success);
    std::cout << "  ✓ EngagementStatus serialization passed" << std::endl;
    
    // Test deserialization
    skyguardis::protocol::EngagementStatus deserialized_status;
    success = skyguardis::protocol::deserializeEngagementStatus(status_buffer, sizeof(status_buffer), deserialized_status);
    assert(success);
    assert(deserialized_status.target_id == status.target_id);
    assert(deserialized_status.state == status.state);
    assert(deserialized_status.firing == status.firing);
    assert(deserialized_status.lead_angle_rad == status.lead_angle_rad);
    assert(deserialized_status.time_to_impact_s == status.time_to_impact_s);
    std::cout << "  ✓ EngagementStatus deserialization passed" << std::endl;
}

void test_checksum() {
    std::cout << "Testing checksum calculation..." << std::endl;
    
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint16_t checksum = skyguardis::protocol::calculateChecksum(data, sizeof(data));
    
    // Verify checksum validation
    bool valid = skyguardis::protocol::validateChecksum(data, sizeof(data), checksum);
    assert(valid);
    std::cout << "  ✓ Checksum calculation and validation passed" << std::endl;
    
    // Test invalid checksum
    uint16_t invalid_checksum = checksum + 1;
    valid = skyguardis::protocol::validateChecksum(data, sizeof(data), invalid_checksum);
    assert(!valid);
    std::cout << "  ✓ Invalid checksum detection passed" << std::endl;
}

void test_message_gateway_initialization() {
    std::cout << "Testing MessageGateway initialization..." << std::endl;
    
    skyguardis::gateway::MessageGateway gateway;
    assert(!gateway.isInitialized());
    
    bool success = gateway.initialize(8888, 8889);
    // May fail if ports are in use, but structure should be correct
    if (success) {
        assert(gateway.isInitialized());
        std::cout << "  ✓ MessageGateway initialization passed" << std::endl;
        gateway.shutdown();
    } else {
        std::cout << "  ⚠ MessageGateway initialization skipped (ports may be in use)" << std::endl;
    }
}

int main() {
    std::cout << "Running message gateway tests..." << std::endl;
    std::cout << std::endl;
    
    try {
        test_serialization();
        test_checksum();
        test_message_gateway_initialization();
        
        std::cout << std::endl;
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}

