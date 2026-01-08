#include "radar_simulator/radar_simulator.hpp"
#include <algorithm>
#include <chrono>

namespace skyguardis {
namespace radar {

RadarSimulator::RadarSimulator() : initialized_(false) {
    scenario_manager_ = std::make_unique<ScenarioManager>();
    initialize();
}

RadarSimulator::~RadarSimulator() {}

void RadarSimulator::initialize() {
    if (!initialized_) {
        last_update_time_ = std::chrono::steady_clock::now();
        scenario_manager_->setScenario(ScenarioType::SINGLE_TARGET);
        initialized_ = true;
    }
}

void RadarSimulator::setScenario(ScenarioType type, const ScenarioConfig& config) {
    scenario_manager_->setScenario(type, config);
    track_states_ = scenario_manager_->generateInitialTracks();
    last_update_time_ = std::chrono::steady_clock::now();
}

ScenarioType RadarSimulator::getCurrentScenario() const {
    return scenario_manager_->getCurrentScenario();
}

void RadarSimulator::generateTracks() {
    if (!initialized_) {
        initialize();
    }
    
    // Generate initial tracks if empty
    if (track_states_.empty()) {
        track_states_ = scenario_manager_->generateInitialTracks();
    }
}

void RadarSimulator::updateTracks(double delta_time_s) {
    if (!initialized_) {
        initialize();
    }
    
    // Calculate actual delta time if not provided
    if (delta_time_s <= 0.0) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time - last_update_time_);
        delta_time_s = elapsed.count() / 1000.0;
        last_update_time_ = current_time;
    } else {
        last_update_time_ = std::chrono::steady_clock::now();
    }
    
    // Update tracks using scenario manager
    scenario_manager_->updateTracks(track_states_, delta_time_s);
}

std::vector<c2::Track> RadarSimulator::getCurrentTracks() const {
    std::vector<c2::Track> tracks;
    
    for (const auto& track_state : track_states_) {
        if (track_state.active) {
            tracks.push_back(track_state.track);
        }
    }
    
    return tracks;
}

size_t RadarSimulator::getActiveTrackCount() const {
    return std::count_if(track_states_.begin(), track_states_.end(),
                         [](const TrackState& ts) { return ts.active; });
}

} // namespace radar
} // namespace skyguardis
