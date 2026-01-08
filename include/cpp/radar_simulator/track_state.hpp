#pragma once

#include "c2_controller/threat_evaluator.hpp"
#include <chrono>
#include <vector>
#include <memory>

namespace skyguardis {
namespace radar {

// Motion model types
enum class MotionModel {
    LINEAR,      // Constant velocity
    MANEUVERING  // Velocity changes with acceleration
};

// Track state with history and motion model
struct TrackState {
    c2::Track track;
    MotionModel motion_model;
    
    // Motion parameters
    double acceleration_ms2;      // Acceleration for maneuvering
    double angular_velocity_rads;  // Turn rate
    double last_update_time_s;     // Time of last update
    
    // Track lifecycle
    bool active;                  // Is track currently active
    uint32_t age_cycles;          // Number of cycles track has existed
    double entry_time_s;          // Time when track entered detection zone
    
    // Position history (for prediction)
    std::vector<c2::Track> history;
    static constexpr size_t MAX_HISTORY = 10;
    
    TrackState() : motion_model(MotionModel::LINEAR),
                   acceleration_ms2(0.0),
                   angular_velocity_rads(0.0),
                   last_update_time_s(0.0),
                   active(true),
                   age_cycles(0),
                   entry_time_s(0.0) {}
};

} // namespace radar
} // namespace skyguardis

