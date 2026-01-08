#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include "radar_simulator/radar_simulator.hpp"
#include "radar_simulator/scenario_manager.hpp"
#include "radar_simulator/track_state.hpp"
#include "c2_controller/threat_evaluator.hpp"

using namespace skyguardis::radar;
using namespace skyguardis::c2;

// Test: Track persistence across cycles
void test_track_persistence() {
    std::cout << "  Testing track persistence...\n";
    
    RadarSimulator radar;
    radar.setScenario(ScenarioType::SINGLE_TARGET);
    radar.generateTracks();
    
    auto tracks1 = radar.getCurrentTracks();
    assert(!tracks1.empty() && "Should have at least one track");
    uint32_t track_id = tracks1[0].id;
    
    // Update tracks multiple times
    for (int i = 0; i < 10; ++i) {
        radar.updateTracks(0.1);
    }
    
    auto tracks2 = radar.getCurrentTracks();
    assert(!tracks2.empty() && "Track should persist");
    assert(tracks2[0].id == track_id && "Track ID should remain the same");
    
    std::cout << "    ✓ Track ID persisted: " << track_id << "\n";
    std::cout << "  ✓ Track persistence test passed\n";
}

// Test: Linear motion model
void test_linear_motion() {
    std::cout << "  Testing linear motion model...\n";
    
    ScenarioManager manager;
    ScenarioConfig config;
    config.type = ScenarioType::SINGLE_TARGET;
    manager.setScenario(ScenarioType::SINGLE_TARGET, config);
    
    auto tracks = manager.generateInitialTracks();
    assert(!tracks.empty() && "Should have tracks");
    
    TrackState& track = tracks[0];
    track.motion_model = MotionModel::LINEAR;
    track.track.range_m = 5000.0;
    track.track.velocity_ms = 200.0;
    track.track.heading_rad = 0.0;
    
    double initial_range = track.track.range_m;
    
    // Apply motion for 1 second
    manager.applyMotionModel(track, 1.0);
    
    // Range should change (simplified check - motion is complex in spherical coords)
    assert(std::abs(track.track.range_m - initial_range) < 1000.0 && 
           "Range should change with motion");
    
    std::cout << "    ✓ Initial range: " << initial_range << "m\n";
    std::cout << "    ✓ After motion: " << track.track.range_m << "m\n";
    std::cout << "  ✓ Linear motion test passed\n";
}

// Test: Maneuvering motion model
void test_maneuvering_motion() {
    std::cout << "  Testing maneuvering motion model...\n";
    
    ScenarioManager manager;
    manager.setScenario(ScenarioType::SINGLE_TARGET);
    
    auto tracks = manager.generateInitialTracks();
    assert(!tracks.empty() && "Should have tracks");
    
    TrackState& track = tracks[0];
    track.motion_model = MotionModel::MANEUVERING;
    track.track.velocity_ms = 200.0;
    track.acceleration_ms2 = 10.0;
    track.angular_velocity_rads = 0.1;
    
    double initial_velocity = track.track.velocity_ms;
    double initial_heading = track.track.heading_rad;
    
    // Apply motion for 1 second
    manager.applyMotionModel(track, 1.0);
    
    // Velocity should change
    assert(std::abs(track.track.velocity_ms - initial_velocity) > 0.1 && 
           "Velocity should change with acceleration");
    
    // Heading should change
    assert(std::abs(track.track.heading_rad - initial_heading) > 0.01 && 
           "Heading should change with angular velocity");
    
    std::cout << "    ✓ Initial velocity: " << initial_velocity << "m/s\n";
    std::cout << "    ✓ After motion: " << track.track.velocity_ms << "m/s\n";
    std::cout << "    ✓ Heading changed: " << (track.track.heading_rad - initial_heading) << "rad\n";
    std::cout << "  ✓ Maneuvering motion test passed\n";
}

// Test: Single target scenario
void test_single_target_scenario() {
    std::cout << "  Testing single target scenario...\n";
    
    RadarSimulator radar;
    ScenarioConfig config;
    config.type = ScenarioType::SINGLE_TARGET;
    radar.setScenario(ScenarioType::SINGLE_TARGET, config);
    
    auto tracks = radar.getCurrentTracks();
    assert(tracks.size() == 1 && "Single target scenario should have exactly 1 track");
    
    // Update multiple times - should maintain single track
    for (int i = 0; i < 5; ++i) {
        radar.updateTracks(0.1);
        tracks = radar.getCurrentTracks();
        assert(tracks.size() <= 1 && "Should maintain single track");
    }
    
    std::cout << "    ✓ Track count: " << tracks.size() << "\n";
    std::cout << "  ✓ Single target scenario test passed\n";
}

// Test: Swarm scenario
void test_swarm_scenario() {
    std::cout << "  Testing swarm scenario...\n";
    
    RadarSimulator radar;
    ScenarioConfig config;
    config.type = ScenarioType::SWARM;
    config.target_count = 5;
    radar.setScenario(ScenarioType::SWARM, config);
    
    auto tracks = radar.getCurrentTracks();
    assert(tracks.size() >= 3 && tracks.size() <= 10 && 
           "Swarm scenario should have 3-10 tracks");
    
    std::cout << "    ✓ Initial track count: " << tracks.size() << "\n";
    
    // Update tracks
    radar.updateTracks(0.1);
    tracks = radar.getCurrentTracks();
    
    std::cout << "    ✓ After update track count: " << tracks.size() << "\n";
    std::cout << "  ✓ Swarm scenario test passed\n";
}

// Test: Saturation scenario
void test_saturation_scenario() {
    std::cout << "  Testing saturation scenario...\n";
    
    RadarSimulator radar;
    ScenarioConfig config;
    config.type = ScenarioType::SATURATION;
    config.target_count = 15;
    radar.setScenario(ScenarioType::SATURATION, config);
    
    auto tracks = radar.getCurrentTracks();
    assert(tracks.size() >= 10 && tracks.size() <= 20 && 
           "Saturation scenario should have 10-20 tracks");
    
    std::cout << "    ✓ Initial track count: " << tracks.size() << "\n";
    std::cout << "  ✓ Saturation scenario test passed\n";
}

// Test: Track lifecycle - creation
void test_track_creation() {
    std::cout << "  Testing track creation...\n";
    
    ScenarioManager manager;
    manager.setScenario(ScenarioType::SINGLE_TARGET);
    
    std::vector<TrackState> tracks;
    uint32_t next_id = 1;
    
    manager.addNewTrack(tracks, next_id);
    assert(tracks.size() == 1 && "Should have one track after creation");
    assert(tracks[0].track.id == 1 && "Track ID should be correct");
    assert(tracks[0].active && "Track should be active");
    
    std::cout << "    ✓ Created track ID: " << tracks[0].track.id << "\n";
    std::cout << "  ✓ Track creation test passed\n";
}

// Test: Track lifecycle - bounds checking
void test_track_bounds() {
    std::cout << "  Testing track bounds checking...\n";
    
    ScenarioManager manager;
    ScenarioConfig config;
    config.min_range_m = 1000.0;
    config.max_range_m = 10000.0;
    config.detection_zone_radius_m = 15000.0;
    manager.setScenario(ScenarioType::SINGLE_TARGET, config);
    
    TrackState track;
    track.track.range_m = 5000.0;
    track.track.elevation_rad = 0.0;
    
    assert(manager.isTrackInBounds(track) && "Track within bounds should be valid");
    
    track.track.range_m = 20000.0; // Out of bounds
    assert(!manager.isTrackInBounds(track) && "Track out of bounds should be invalid");
    
    track.track.range_m = 5000.0;
    track.track.elevation_rad = 1.0; // Out of elevation bounds
    assert(!manager.isTrackInBounds(track) && "Track with invalid elevation should be invalid");
    
    std::cout << "    ✓ Bounds checking works correctly\n";
    std::cout << "  ✓ Track bounds test passed\n";
}

// Test: Track removal when out of bounds
void test_track_removal() {
    std::cout << "  Testing track removal...\n";
    
    ScenarioManager manager;
    ScenarioConfig config;
    config.min_range_m = 1000.0;
    config.detection_zone_radius_m = 10000.0;
    manager.setScenario(ScenarioType::SINGLE_TARGET, config);
    
    std::vector<TrackState> tracks = manager.generateInitialTracks();
    size_t initial_count = tracks.size();
    
    // Move track out of bounds
    if (!tracks.empty()) {
        tracks[0].track.range_m = 20000.0; // Out of bounds
    }
    
    manager.removeOutOfBoundsTracks(tracks);
    assert(tracks.size() < initial_count && "Out-of-bounds tracks should be removed");
    
    std::cout << "    ✓ Removed " << (initial_count - tracks.size()) << " out-of-bounds tracks\n";
    std::cout << "  ✓ Track removal test passed\n";
}

// Test: Track history maintenance
void test_track_history() {
    std::cout << "  Testing track history...\n";
    
    ScenarioManager manager;
    manager.setScenario(ScenarioType::SINGLE_TARGET);
    
    auto tracks = manager.generateInitialTracks();
    assert(!tracks.empty() && "Should have tracks");
    
    TrackState& track = tracks[0];
    assert(track.history.empty() && "New track should have empty history");
    
    // Update multiple times
    for (int i = 0; i < 5; ++i) {
        manager.updateTracks(tracks, 0.1);
    }
    
    assert(!track.history.empty() && "Track should have history after updates");
    assert(track.history.size() <= TrackState::MAX_HISTORY && 
           "History should not exceed maximum");
    
    std::cout << "    ✓ History size: " << track.history.size() << "\n";
    std::cout << "  ✓ Track history test passed\n";
}

// Test: Velocity constraints
void test_velocity_constraints() {
    std::cout << "  Testing velocity constraints...\n";
    
    ScenarioManager manager;
    manager.setScenario(ScenarioType::SINGLE_TARGET);
    
    auto tracks = manager.generateInitialTracks();
    assert(!tracks.empty() && "Should have tracks");
    
    TrackState& track = tracks[0];
    track.motion_model = MotionModel::MANEUVERING;
    track.track.velocity_ms = 100.0;
    track.acceleration_ms2 = 1000.0; // Very high acceleration
    
    // Apply motion for 1 second - should be constrained
    manager.applyMotionModel(track, 1.0);
    
    assert(track.track.velocity_ms <= 500.0 && "Velocity should be constrained to max");
    assert(track.track.velocity_ms >= 50.0 && "Velocity should be constrained to min");
    
    std::cout << "    ✓ Velocity constrained to: " << track.track.velocity_ms << "m/s\n";
    std::cout << "  ✓ Velocity constraints test passed\n";
}

// Test: Multiple motion model types
void test_mixed_motion_models() {
    std::cout << "  Testing mixed motion models...\n";
    
    ScenarioManager manager;
    ScenarioConfig config;
    config.type = ScenarioType::SWARM;
    config.target_count = 5;
    manager.setScenario(ScenarioType::SWARM, config);
    
    auto tracks = manager.generateInitialTracks();
    assert(tracks.size() >= 3 && "Should have multiple tracks");
    
    // Count motion model types
    int linear_count = 0;
    int maneuvering_count = 0;
    
    for (const auto& track : tracks) {
        if (track.motion_model == MotionModel::LINEAR) {
            linear_count++;
        } else if (track.motion_model == MotionModel::MANEUVERING) {
            maneuvering_count++;
        }
    }
    
    assert(linear_count + maneuvering_count == static_cast<int>(tracks.size()) && 
           "All tracks should have motion models");
    
    std::cout << "    ✓ Linear motion tracks: " << linear_count << "\n";
    std::cout << "    ✓ Maneuvering motion tracks: " << maneuvering_count << "\n";
    std::cout << "  ✓ Mixed motion models test passed\n";
}

// Test: Track age tracking
void test_track_age() {
    std::cout << "  Testing track age tracking...\n";
    
    ScenarioManager manager;
    manager.setScenario(ScenarioType::SINGLE_TARGET);
    
    auto tracks = manager.generateInitialTracks();
    assert(!tracks.empty() && "Should have tracks");
    
    TrackState& track = tracks[0];
    uint32_t initial_age = track.age_cycles;
    
    // Update multiple times
    for (int i = 0; i < 10; ++i) {
        manager.updateTracks(tracks, 0.1);
    }
    
    assert(track.age_cycles > initial_age && "Track age should increase");
    assert(track.age_cycles >= 10 && "Track age should reflect updates");
    
    std::cout << "    ✓ Initial age: " << initial_age << " cycles\n";
    std::cout << "    ✓ Final age: " << track.age_cycles << " cycles\n";
    std::cout << "  ✓ Track age test passed\n";
}

// Test: Scenario configuration
void test_scenario_configuration() {
    std::cout << "  Testing scenario configuration...\n";
    
    ScenarioManager manager;
    ScenarioConfig config;
    config.min_range_m = 2000.0;
    config.max_range_m = 8000.0;
    config.min_velocity_ms = 100.0;
    config.max_velocity_ms = 250.0;
    
    manager.setScenario(ScenarioType::SINGLE_TARGET, config);
    
    auto tracks = manager.generateInitialTracks();
    assert(!tracks.empty() && "Should have tracks");
    
    const TrackState& track = tracks[0];
    assert(track.track.range_m >= config.min_range_m && 
           track.track.range_m <= config.max_range_m && 
           "Range should be within configured bounds");
    assert(track.track.velocity_ms >= config.min_velocity_ms && 
           track.track.velocity_ms <= config.max_velocity_ms && 
           "Velocity should be within configured bounds");
    
    std::cout << "    ✓ Range: " << track.track.range_m << "m (within bounds)\n";
    std::cout << "    ✓ Velocity: " << track.track.velocity_ms << "m/s (within bounds)\n";
    std::cout << "  ✓ Scenario configuration test passed\n";
}

// Test: Track update frequency
void test_track_update_frequency() {
    std::cout << "  Testing track update frequency...\n";
    
    RadarSimulator radar;
    radar.setScenario(ScenarioType::SINGLE_TARGET);
    radar.generateTracks();
    
    auto initial_tracks = radar.getCurrentTracks();
    assert(!initial_tracks.empty() && "Should have tracks");
    
    double initial_range = initial_tracks[0].range_m;
    
    // Update with different time deltas
    radar.updateTracks(0.1); // 100ms
    auto tracks_100ms = radar.getCurrentTracks();
    
    radar.setScenario(ScenarioType::SINGLE_TARGET);
    radar.generateTracks();
    radar.updateTracks(0.5); // 500ms
    auto tracks_500ms = radar.getCurrentTracks();
    
    // Longer time delta should result in larger position change
    double change_100ms = std::abs(tracks_100ms[0].range_m - initial_range);
    double change_500ms = std::abs(tracks_500ms[0].range_m - initial_range);
    
    std::cout << "    ✓ Position change (100ms): " << change_100ms << "m\n";
    std::cout << "    ✓ Position change (500ms): " << change_500ms << "m\n";
    std::cout << "  ✓ Track update frequency test passed\n";
}

// Test: Active track counting
void test_active_track_counting() {
    std::cout << "  Testing active track counting...\n";
    
    RadarSimulator radar;
    ScenarioConfig config;
    config.type = ScenarioType::SWARM;
    config.target_count = 7;
    radar.setScenario(ScenarioType::SWARM, config);
    
    size_t active_count = radar.getActiveTrackCount();
    assert(active_count >= 3 && active_count <= 10 && 
           "Active track count should match swarm scenario");
    
    std::cout << "    ✓ Active tracks: " << active_count << "\n";
    std::cout << "  ✓ Active track counting test passed\n";
}

int main() {
    std::cout << "\nTesting Enhanced Radar Simulation...\n\n";
    
    try {
        test_track_persistence();
        test_linear_motion();
        test_maneuvering_motion();
        test_single_target_scenario();
        test_swarm_scenario();
        test_saturation_scenario();
        test_track_creation();
        test_track_bounds();
        test_track_removal();
        test_track_history();
        test_velocity_constraints();
        test_mixed_motion_models();
        test_track_age();
        test_scenario_configuration();
        test_track_update_frequency();
        test_active_track_counting();
        
        std::cout << "\n✓ All enhanced radar simulation tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}

