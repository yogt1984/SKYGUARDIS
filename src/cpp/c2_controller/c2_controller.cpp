#include "c2_controller/c2_controller.hpp"
#include "c2_controller/threat_evaluator.hpp"
#include "message_gateway/protocol.hpp"
#include "message_gateway/message_gateway.hpp"

namespace skyguardis {
namespace c2 {

void C2Controller::processTracks(const std::vector<Track>& tracks) {
    if (tracks.empty()) {
        return;
    }
    
    // Evaluate and prioritize threats
    auto prioritized = evaluator_.prioritize(tracks);
    
    // Assign highest priority target
    if (!prioritized.empty()) {
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
    // In real implementation, would send via message gateway
    // For now, just log the assignment
}

} // namespace c2
} // namespace skyguardis

