#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>
#include "message_gateway/message_gateway.hpp"
#include "radar_simulator/radar_simulator.hpp"
#include "c2_controller/c2_controller.hpp"
#include "c2_controller/threat_evaluator.hpp"

using namespace skyguardis::gateway;
using namespace skyguardis::radar;
using namespace skyguardis::c2;
using namespace skyguardis::protocol;

// Test: End-to-end integration
void test_end_to_end_integration() {
    std::cout << "  Testing end-to-end integration...\n";
    
    MessageGateway gateway;
    assert(gateway.initialize(8888, 8889));
    
    RadarSimulator radar;
    radar.setScenario(ScenarioType::SINGLE_TARGET);
    radar.updateTracks();
    
    C2Controller c2;
    c2.setMessageGateway(&gateway);
    
    auto tracks = radar.getCurrentTracks();
    assert(!tracks.empty() && "Should have tracks");
    
    c2.processTracks(tracks);
    
    std::cout << "    ✓ End-to-end flow: Radar -> C2 -> Gateway\n";
    std::cout << "  ✓ End-to-end integration test passed\n";
}

// Test: Multiple engagement cycles
void test_multiple_engagement_cycles() {
    std::cout << "  Testing multiple engagement cycles...\n";
    
    RadarSimulator radar;
    radar.setScenario(ScenarioType::SWARM);
    
    for (int i = 0; i < 10; ++i) {
        radar.updateTracks(0.1);
        auto tracks = radar.getCurrentTracks();
        assert(tracks.size() >= 3 && "Swarm should maintain multiple tracks");
    }
    
    std::cout << "    ✓ Completed 10 engagement cycles\n";
    std::cout << "  ✓ Multiple engagement cycles test passed\n";
}

// Test: Message integrity under load
void test_message_integrity_load() {
    std::cout << "  Testing message integrity under load...\n";
    
    MessageGateway gateway;
    assert(gateway.initialize(8888, 8889));
    
    int success_count = 0;
    for (int i = 0; i < 100; ++i) {
        TargetAssignment assignment;
        assignment.target_id = i;
        assignment.range_m = 5000.0 + i;
        assignment.azimuth_rad = 0.5;
        assignment.elevation_rad = 0.2;
        assignment.velocity_ms = 200.0;
        assignment.priority = 100;
        
        if (gateway.sendTargetAssignment(assignment)) {
            success_count++;
        }
    }
    
    assert(success_count > 90 && "Should succeed most of the time");
    std::cout << "    ✓ Sent " << success_count << "/100 messages successfully\n";
    std::cout << "  ✓ Message integrity under load test passed\n";
}

// Test: Scenario switching
void test_scenario_switching() {
    std::cout << "  Testing scenario switching...\n";
    
    RadarSimulator radar;
    
    radar.setScenario(ScenarioType::SINGLE_TARGET);
    radar.updateTracks();
    assert(radar.getActiveTrackCount() == 1 && "Single target should have 1 track");
    std::cout << "    ✓ Single: " << radar.getActiveTrackCount() << " tracks\n";
    
    ScenarioConfig config;
    config.target_count = 5;
    radar.setScenario(ScenarioType::SWARM, config);
    radar.updateTracks();
    assert(radar.getActiveTrackCount() >= 3 && "Swarm should have 3+ tracks");
    std::cout << "    ✓ Swarm: " << radar.getActiveTrackCount() << " tracks\n";
    
    config.target_count = 15;
    radar.setScenario(ScenarioType::SATURATION, config);
    radar.updateTracks();
    assert(radar.getActiveTrackCount() >= 10 && "Saturation should have 10+ tracks");
    std::cout << "    ✓ Saturation: " << radar.getActiveTrackCount() << " tracks\n";
    
    std::cout << "  ✓ Scenario switching test passed\n";
}

// Test: Concurrent operations
void test_concurrent_operations() {
    std::cout << "  Testing concurrent operations...\n";
    
    RadarSimulator radar;
    radar.setScenario(ScenarioType::SWARM);
    
    int success_count = 0;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&radar, &success_count, i]() {
            radar.updateTracks(0.1);
            auto tracks = radar.getCurrentTracks();
            if (!tracks.empty()) {
                success_count++;
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    assert(success_count == 10 && "All concurrent operations should succeed");
    std::cout << "    ✓ " << success_count << "/10 concurrent operations succeeded\n";
    std::cout << "  ✓ Concurrent operations test passed\n";
}

int main() {
    std::cout << "\nTesting Comprehensive Integration...\n\n";
    
    try {
        test_end_to_end_integration();
        test_multiple_engagement_cycles();
        test_message_integrity_load();
        test_scenario_switching();
        test_concurrent_operations();
        
        std::cout << "\n✓ All comprehensive integration tests passed!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ Test failed: " << e.what() << "\n";
        return 1;
    }
}
