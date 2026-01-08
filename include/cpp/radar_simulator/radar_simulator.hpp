#pragma once

#include "c2_controller/threat_evaluator.hpp"
#include "radar_simulator/scenario_manager.hpp"
#include "radar_simulator/track_state.hpp"
#include <vector>
#include <memory>
#include <chrono>

namespace skyguardis {
namespace radar {

class RadarSimulator {
public:
    RadarSimulator();
    ~RadarSimulator();
    
    // Track generation and updates
    void generateTracks();
    void updateTracks(double delta_time_s = 0.1); // Default 100ms cycle
    std::vector<c2::Track> getCurrentTracks() const;
    
    // Scenario management
    void setScenario(ScenarioType type, const ScenarioConfig& config = ScenarioConfig());
    ScenarioType getCurrentScenario() const;
    
    // Track state access (for testing)
    const std::vector<TrackState>& getTrackStates() const { return track_states_; }
    size_t getActiveTrackCount() const;

private:
    std::vector<TrackState> track_states_;
    std::unique_ptr<ScenarioManager> scenario_manager_;
    std::chrono::steady_clock::time_point last_update_time_;
    bool initialized_;
    
    void initialize();
};

} // namespace radar
} // namespace skyguardis

