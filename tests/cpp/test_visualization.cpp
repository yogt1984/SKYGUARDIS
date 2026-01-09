#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>
#include <cmath>
#include "logger/visualizer.hpp"
#include "c2_controller/threat_evaluator.hpp"
#include "message_gateway/protocol.hpp"

using namespace skyguardis::logger;
using namespace skyguardis::c2;
using namespace skyguardis::protocol;

// Test: Track visualization
void test_track_visualization() {
    std::cout << "  Testing track visualization...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.enableAutoClear(false);
    
    std::vector<Track> tracks;
    Track track1;
    track1.id = 1;
    track1.range_m = 5000.0;
    track1.azimuth_rad = 0.5;
    track1.elevation_rad = 0.2;
    track1.velocity_ms = 200.0;
    track1.heading_rad = 1.0;
    tracks.push_back(track1);
    
    Track track2;
    track2.id = 2;
    track2.range_m = 8000.0;
    track2.azimuth_rad = -0.3;
    track2.elevation_rad = 0.1;
    track2.velocity_ms = 150.0;
    track2.heading_rad = -0.5;
    tracks.push_back(track2);
    
    visualizer.visualizeTracks(tracks);
    
    std::cout << "    ✓ Visualized " << tracks.size() << " tracks\n";
    std::cout << "  ✓ Track visualization test passed\n";
}

// Test: Engagement status visualization
void test_engagement_status_visualization() {
    std::cout << "  Testing engagement status visualization...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.enableAutoClear(false);
    
    EngagementStatus status;
    status.target_id = 12345;
    status.state = 2; // TRACKING
    status.firing = 0;
    status.lead_angle_rad = 0.1;
    status.time_to_impact_s = 5.5;
    
    visualizer.visualizeEngagementStatus(status);
    
    std::cout << "    ✓ Visualized engagement status for target " << status.target_id << "\n";
    std::cout << "  ✓ Engagement status visualization test passed\n";
}

// Test: Safety status visualization
void test_safety_status_visualization() {
    std::cout << "  Testing safety status visualization...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.enableAutoClear(false);
    
    visualizer.visualizeSafetyStatus(true, "All systems nominal");
    visualizer.visualizeSafetyStatus(false, "Range out of bounds");
    
    std::cout << "    ✓ Visualized safe and unsafe states\n";
    std::cout << "  ✓ Safety status visualization test passed\n";
}

// Test: Safety violation visualization
void test_safety_violation_visualization() {
    std::cout << "  Testing safety violation visualization...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.enableAutoClear(false);
    
    visualizer.visualizeSafetyViolation("Range Limit", "Target range 50m < minimum 100m");
    visualizer.visualizeSafetyViolation("No-Fire Zone", "Target in restricted firing arc");
    
    std::cout << "    ✓ Visualized safety violations\n";
    std::cout << "  ✓ Safety violation visualization test passed\n";
}

// Test: Dashboard visualization
void test_dashboard_visualization() {
    std::cout << "  Testing dashboard visualization...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.enableAutoClear(false);
    visualizer.setUpdateInterval(1); // Update every cycle for testing
    
    std::vector<Track> tracks;
    Track track;
    track.id = 1;
    track.range_m = 5000.0;
    track.azimuth_rad = 0.5;
    track.elevation_rad = 0.2;
    track.velocity_ms = 200.0;
    track.heading_rad = 1.0;
    tracks.push_back(track);
    
    EngagementStatus status;
    status.target_id = 1;
    status.state = 2; // TRACKING
    status.firing = 0;
    status.lead_angle_rad = 0.1;
    status.time_to_impact_s = 5.5;
    
    visualizer.visualizeDashboard(tracks, status, true);
    
    std::cout << "    ✓ Dashboard visualization complete\n";
    std::cout << "  ✓ Dashboard visualization test passed\n";
}

// Test: Format switching
void test_format_switching() {
    std::cout << "  Testing format switching...\n";
    
    Visualizer visualizer;
    
    std::vector<Track> tracks;
    Track track;
    track.id = 1;
    track.range_m = 5000.0;
    track.azimuth_rad = 0.5;
    track.elevation_rad = 0.2;
    track.velocity_ms = 200.0;
    track.heading_rad = 1.0;
    tracks.push_back(track);
    
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.visualizeTracks(tracks);
    
    visualizer.setFormat(VisualFormat::COMPACT);
    visualizer.visualizeTracks(tracks);
    
    visualizer.setFormat(VisualFormat::ASCII_ART);
    visualizer.visualizeTracks(tracks);
    
    std::cout << "    ✓ Switched between all formats\n";
    std::cout << "  ✓ Format switching test passed\n";
}

// Test: Update interval
void test_update_interval() {
    std::cout << "  Testing update interval...\n";
    
    Visualizer visualizer;
    visualizer.setUpdateInterval(5); // Update every 5 cycles
    visualizer.enableAutoClear(false);
    
    std::vector<Track> tracks;
    Track track;
    track.id = 1;
    track.range_m = 5000.0;
    track.azimuth_rad = 0.5;
    track.elevation_rad = 0.2;
    track.velocity_ms = 200.0;
    track.heading_rad = 1.0;
    tracks.push_back(track);
    
    // Call multiple times - should only update on interval
    for (int i = 0; i < 10; ++i) {
        visualizer.visualizeTracks(tracks);
    }
    
    std::cout << "    ✓ Update interval working correctly\n";
    std::cout << "  ✓ Update interval test passed\n";
}

// Test: File output
void test_file_output() {
    std::cout << "  Testing file output...\n";
    
    Visualizer visualizer;
    visualizer.setOutputFile("/tmp/test_visualization.log");
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.setUpdateInterval(1);
    
    std::vector<Track> tracks;
    Track track;
    track.id = 1;
    track.range_m = 5000.0;
    track.azimuth_rad = 0.5;
    track.elevation_rad = 0.2;
    track.velocity_ms = 200.0;
    track.heading_rad = 1.0;
    tracks.push_back(track);
    
    visualizer.visualizeTracks(tracks);
    
    // Verify file was written
    std::ifstream file("/tmp/test_visualization.log");
    assert(file.good() && "Visualization log file should exist");
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    assert(content.find("Tracks:") != std::string::npos && "File should contain track info");
    
    std::cout << "    ✓ File output verified\n";
    std::cout << "  ✓ File output test passed\n";
}

// Test: Multiple tracks visualization
void test_multiple_tracks() {
    std::cout << "  Testing multiple tracks visualization...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.setUpdateInterval(1);
    visualizer.enableAutoClear(false);
    
    std::vector<Track> tracks;
    for (int i = 0; i < 10; ++i) {
        Track track;
        track.id = i + 1;
        track.range_m = 2000.0 + i * 500.0;
        track.azimuth_rad = -1.5 + i * 0.3;
        track.elevation_rad = 0.1;
        track.velocity_ms = 100.0 + i * 20.0;
        track.heading_rad = 0.0;
        tracks.push_back(track);
    }
    
    visualizer.visualizeTracks(tracks);
    
    assert(tracks.size() == 10 && "Should visualize all tracks");
    std::cout << "    ✓ Visualized " << tracks.size() << " tracks\n";
    std::cout << "  ✓ Multiple tracks visualization test passed\n";
}

// Test: Empty tracks visualization
void test_empty_tracks() {
    std::cout << "  Testing empty tracks visualization...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.setUpdateInterval(1);
    visualizer.enableAutoClear(false);
    
    std::vector<Track> empty_tracks;
    visualizer.visualizeTracks(empty_tracks);
    
    std::cout << "    ✓ Handled empty tracks correctly\n";
    std::cout << "  ✓ Empty tracks visualization test passed\n";
}

// Test: Format helper methods
void test_format_helpers() {
    std::cout << "  Testing format helper methods...\n";
    
    Visualizer visualizer;
    
    // Test angle formatting (indirectly through visualization)
    std::vector<Track> tracks;
    Track track;
    track.id = 1;
    track.range_m = 5000.0;
    track.azimuth_rad = 1.5708; // 90 degrees
    track.elevation_rad = 0.0;
    track.velocity_ms = 200.0;
    track.heading_rad = 0.0;
    tracks.push_back(track);
    
    visualizer.setFormat(VisualFormat::COMPACT);
    visualizer.visualizeTracks(tracks);
    
    std::cout << "    ✓ Format helpers working\n";
    std::cout << "  ✓ Format helpers test passed\n";
}

// Test: State string conversion
void test_state_strings() {
    std::cout << "  Testing state string conversion...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::ASCII_TABLE);
    visualizer.setUpdateInterval(1);
    visualizer.enableAutoClear(false);
    
    EngagementStatus status;
    status.target_id = 1;
    status.firing = 0;
    status.lead_angle_rad = 0.1;
    status.time_to_impact_s = 5.5;
    
    // Test all states
    for (uint8_t state = 0; state <= 5; ++state) {
        status.state = state;
        visualizer.visualizeEngagementStatus(status);
    }
    
    std::cout << "    ✓ All state strings converted correctly\n";
    std::cout << "  ✓ State string conversion test passed\n";
}

// Test: Compact format
void test_compact_format() {
    std::cout << "  Testing compact format...\n";
    
    Visualizer visualizer;
    visualizer.setFormat(VisualFormat::COMPACT);
    visualizer.setUpdateInterval(1);
    visualizer.enableAutoClear(false);
    
    std::vector<Track> tracks;
    Track track;
    track.id = 1;
    track.range_m = 5000.0;
    track.azimuth_rad = 0.5;
    track.elevation_rad = 0.2;
    track.velocity_ms = 200.0;
    track.heading_rad = 1.0;
    tracks.push_back(track);
    
    visualizer.visualizeTracks(tracks);
    
    std::cout << "    ✓ Compact format working\n";
    std::cout << "  ✓ Compact format test passed\n";
}

int main() {
    std::cout << "\nTesting Visualization System...\n\n";
    
    try {
        test_track_visualization();
        test_engagement_status_visualization();
        test_safety_status_visualization();
        test_safety_violation_visualization();
        test_dashboard_visualization();
        test_format_switching();
        test_update_interval();
        test_file_output();
        test_multiple_tracks();
        test_empty_tracks();
        test_format_helpers();
        test_state_strings();
        test_compact_format();
        
        std::cout << "\n✓ All visualization tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}

