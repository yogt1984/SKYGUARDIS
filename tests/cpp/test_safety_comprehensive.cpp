#include <iostream>
#include <cassert>
#include <cmath>
#include "c2_controller/threat_evaluator.hpp"

using namespace skyguardis::c2;

// Test: Threat evaluation with various ranges
void test_threat_evaluation_ranges() {
    std::cout << "  Testing threat evaluation with various ranges...\n";
    
    ThreatEvaluator evaluator;
    
    Track close_track;
    close_track.id = 1;
    close_track.range_m = 1000.0;  // Close
    close_track.velocity_ms = 200.0;
    close_track.azimuth_rad = 0.0;
    close_track.elevation_rad = 0.1;
    close_track.heading_rad = 0.0;
    
    Track far_track;
    far_track.id = 2;
    far_track.range_m = 10000.0;  // Far
    far_track.velocity_ms = 200.0;
    far_track.azimuth_rad = 0.0;
    far_track.elevation_rad = 0.1;
    far_track.heading_rad = 0.0;
    
    auto close_score = evaluator.evaluate(close_track);
    auto far_score = evaluator.evaluate(far_track);
    
    assert(close_score.score > far_score.score && "Close threats should score higher");
    
    std::cout << "    ✓ Close threat: " << close_score.score << "\n";
    std::cout << "    ✓ Far threat: " << far_score.score << "\n";
    std::cout << "  ✓ Threat evaluation ranges test passed\n";
}

// Test: Threat evaluation with various velocities
void test_threat_evaluation_velocities() {
    std::cout << "  Testing threat evaluation with various velocities...\n";
    
    ThreatEvaluator evaluator;
    
    Track fast_track;
    fast_track.id = 1;
    fast_track.range_m = 5000.0;
    fast_track.velocity_ms = 400.0;  // Fast
    fast_track.azimuth_rad = 0.0;
    fast_track.elevation_rad = 0.1;
    fast_track.heading_rad = 0.0;
    
    Track slow_track;
    slow_track.id = 2;
    slow_track.range_m = 5000.0;
    slow_track.velocity_ms = 50.0;  // Slow
    slow_track.azimuth_rad = 0.0;
    slow_track.elevation_rad = 0.1;
    slow_track.heading_rad = 0.0;
    
    auto fast_score = evaluator.evaluate(fast_track);
    auto slow_score = evaluator.evaluate(slow_track);
    
    assert(fast_score.score > slow_score.score && "Fast threats should score higher");
    
    std::cout << "    ✓ Fast threat: " << fast_score.score << "\n";
    std::cout << "    ✓ Slow threat: " << slow_score.score << "\n";
    std::cout << "  ✓ Threat evaluation velocities test passed\n";
}

// Test: Prioritization with multiple tracks
void test_prioritization_multiple() {
    std::cout << "  Testing prioritization with multiple tracks...\n";
    
    ThreatEvaluator evaluator;
    std::vector<Track> tracks;
    
    for (int i = 0; i < 10; ++i) {
        Track track;
        track.id = i + 1;
        track.range_m = 2000.0 + i * 500.0;
        track.velocity_ms = 100.0 + i * 20.0;
        track.azimuth_rad = 0.0;
        track.elevation_rad = 0.1;
        track.heading_rad = 0.0;
        tracks.push_back(track);
    }
    
    auto prioritized = evaluator.prioritize(tracks);
    assert(prioritized.size() == tracks.size() && "Should prioritize all tracks");
    assert(prioritized[0].score >= prioritized[1].score && "Should be sorted by score");
    
    std::cout << "    ✓ Prioritized " << prioritized.size() << " tracks\n";
    std::cout << "    ✓ Highest priority: Track " << prioritized[0].track_id 
              << " (score: " << prioritized[0].score << ")\n";
    std::cout << "  ✓ Prioritization multiple test passed\n";
}

// Test: Threat evaluation edge cases
void test_threat_evaluation_edge_cases() {
    std::cout << "  Testing threat evaluation edge cases...\n";
    
    ThreatEvaluator evaluator;
    
    // Edge case: Very close track
    Track very_close;
    very_close.id = 1;
    very_close.range_m = 100.0;
    very_close.velocity_ms = 200.0;
    very_close.azimuth_rad = 0.0;
    very_close.elevation_rad = 0.1;
    very_close.heading_rad = 0.0;
    
    auto score1 = evaluator.evaluate(very_close);
    assert(score1.score > 0 && "Should handle very close tracks");
    
    // Edge case: Very far track
    Track very_far;
    very_far.id = 2;
    very_far.range_m = 50000.0;
    very_far.velocity_ms = 200.0;
    very_far.azimuth_rad = 0.0;
    very_far.elevation_rad = 0.1;
    very_far.heading_rad = 0.0;
    
    auto score2 = evaluator.evaluate(very_far);
    assert(score2.score >= 0 && "Should handle very far tracks");
    
    std::cout << "    ✓ Handled edge cases successfully\n";
    std::cout << "  ✓ Threat evaluation edge cases test passed\n";
}

int main() {
    std::cout << "\nTesting Comprehensive Safety & Threat Evaluation...\n\n";
    
    try {
        test_threat_evaluation_ranges();
        test_threat_evaluation_velocities();
        test_prioritization_multiple();
        test_threat_evaluation_edge_cases();
        
        std::cout << "\n✓ All comprehensive safety tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}
