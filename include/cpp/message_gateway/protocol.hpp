#pragma once

#include <cstdint>
#include <string>

namespace skyguardis {
namespace protocol {

// Message types
enum class MessageType : uint8_t {
    TARGET_ASSIGNMENT = 1,
    ENGAGEMENT_STATUS = 2,
    SAFETY_INTERLOCK = 3,
    HEARTBEAT = 4
};

// Target assignment message (C++ -> Ada)
struct TargetAssignment {
    uint32_t target_id;
    double range_m;           // Range in meters
    double azimuth_rad;       // Azimuth in radians
    double elevation_rad;     // Elevation in radians
    double velocity_ms;        // Velocity in m/s
    uint8_t priority;         // Threat priority (0-255)
    
    static constexpr size_t SERIALIZED_SIZE = 43; // 6 header + 37 payload
};

// Engagement status message (Ada -> C++)
struct EngagementStatus {
    uint32_t target_id;
    uint8_t state;            // Engagement state machine state (0-5)
    uint8_t firing;           // 0 = not firing, 1 = firing
    double lead_angle_rad;
    double time_to_impact_s;
    
    static constexpr size_t SERIALIZED_SIZE = 28; // 6 header + 22 payload
};

// Serialization functions
bool serializeTargetAssignment(const TargetAssignment& msg, uint8_t* buffer, size_t buffer_size);
bool deserializeTargetAssignment(const uint8_t* buffer, size_t buffer_size, TargetAssignment& msg);

bool serializeEngagementStatus(const EngagementStatus& msg, uint8_t* buffer, size_t buffer_size);
bool deserializeEngagementStatus(const uint8_t* buffer, size_t buffer_size, EngagementStatus& msg);

uint16_t calculateChecksum(const uint8_t* data, size_t length);
bool validateChecksum(const uint8_t* data, size_t length, uint16_t checksum);

} // namespace protocol
} // namespace skyguardis

