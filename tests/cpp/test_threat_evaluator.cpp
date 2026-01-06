#include "c2_controller/threat_evaluator.hpp"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "Running threat evaluator tests..." << std::endl;
    
    skyguardis::c2::ThreatEvaluator evaluator;
    
    // Test 1: Create a test track
    skyguardis::c2::Track track;
    track.id = 1;
    track.range_m = 5000.0;
    track.azimuth_rad = 0.5;
    track.elevation_rad = 0.2;
    track.velocity_ms = 200.0;
    track.heading_rad = 1.0;
    
    // Test 2: Evaluate threat
    auto score = evaluator.evaluate(track);
    assert(score.track_id == 1);
    assert(score.score > 0.0);
    std::cout << "  ✓ Threat evaluation test passed" << std::endl;
    
    // Test 3: Prioritize multiple tracks
    std::vector<skyguardis::c2::Track> tracks;
    tracks.push_back(track);
    
    track.id = 2;
    track.range_m = 2000.0;  // Closer = higher threat
    track.velocity_ms = 250.0;  // Faster = higher threat
    tracks.push_back(track);
    
    auto prioritized = evaluator.prioritize(tracks);
    assert(prioritized.size() == 2);
    assert(prioritized[0].score >= prioritized[1].score);  // Should be sorted
    std::cout << "  ✓ Threat prioritization test passed" << std::endl;
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

