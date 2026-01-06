#include "radar_simulator/radar_simulator.hpp"
#include <random>
#include <cmath>

namespace skyguardis {
namespace radar {

void RadarSimulator::generateTracks() {
    // Placeholder: generate synthetic tracks
    // In real implementation, would use scenario files or real-time generation
}

std::vector<c2::Track> RadarSimulator::getCurrentTracks() const {
    return tracks_;
}

} // namespace radar
} // namespace skyguardis

