#include "c2_controller/c2_controller.hpp"
#include "c2_controller/threat_evaluator.hpp"
#include "message_gateway/protocol.hpp"
#include "message_gateway/message_gateway.hpp"
#include <iostream>

namespace skyguardis {
namespace c2 {

C2Controller::C2Controller() : gateway_(nullptr) {
}

C2Controller::~C2Controller() {
}

void C2Controller::setMessageGateway(gateway::MessageGateway* gateway) {
    gateway_ = gateway;
}

void C2Controller::processTracks(const std::vector<Track>& tracks) {
    if (tracks.empty()) {
        return;
    }
    
    // Evaluate and prioritize threats
    auto prioritized = evaluator_.prioritize(tracks);
    
    // Assign highest priority target if score exceeds threshold
    if (!prioritized.empty() && prioritized[0].score > 0.5) {
        // Find the track with highest priority
        for (const auto& track : tracks) {
            if (track.id == prioritized[0].track_id) {
                assignTarget(track);
                break;
            }
        }
    }
}

void C2Controller::assignTarget(const Track& track) {
    if (!gateway_ || !gateway_->isInitialized()) {
        return;
    }
    
    // Format target assignment message
    protocol::TargetAssignment assignment;
    assignment.target_id = track.id;
    assignment.range_m = track.range_m;
    assignment.azimuth_rad = track.azimuth_rad;
    assignment.elevation_rad = track.elevation_rad;
    assignment.velocity_ms = track.velocity_ms;
    
    // Get priority from threat evaluation
    auto score = evaluator_.evaluate(track);
    assignment.priority = score.priority;
    
    // Send via gateway
    if (gateway_->sendTargetAssignment(assignment)) {
        std::cout << "[C2] Target assigned: ID=" << assignment.target_id 
                  << " Range=" << assignment.range_m << "m" << std::endl;
    } else {
        std::cerr << "[C2] Failed to send target assignment" << std::endl;
    }
}

} // namespace c2
} // namespace skyguardis

