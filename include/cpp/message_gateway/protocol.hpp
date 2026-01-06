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
};

// Engagement status message (Ada -> C++)
struct EngagementStatus {
    uint32_t target_id;
    uint8_t state;            // Engagement state machine state
    bool firing;
    double lead_angle_rad;
    double time_to_impact_s;
};

} // namespace protocol
} // namespace skyguardis

