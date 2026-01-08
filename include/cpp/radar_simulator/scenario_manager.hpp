#pragma once

#include "radar_simulator/track_state.hpp"
#include <vector>
#include <string>
#include <memory>
#include <random>

namespace skyguardis {
namespace radar {

// Scenario types
enum class ScenarioType {
    SINGLE_TARGET,   // One object moving through engagement zone
    SWARM,           // Multiple objects (3-10) simultaneously
    SATURATION       // Many objects (10-20) with high update rate
};

// Scenario configuration
struct ScenarioConfig {
    ScenarioType type;
    uint32_t target_count;
    double min_range_m;
    double max_range_m;
    double min_velocity_ms;
    double max_velocity_ms;
    double min_elevation_rad;
    double max_elevation_rad;
    double detection_zone_radius_m;
    
    ScenarioConfig() : type(ScenarioType::SINGLE_TARGET),
                       target_count(1),
                       min_range_m(1000.0),
                       max_range_m(10000.0),
                       min_velocity_ms(50.0),
                       max_velocity_ms(300.0),
                       min_elevation_rad(-0.5),
                       max_elevation_rad(0.5),
                       detection_zone_radius_m(15000.0) {}
};

class ScenarioManager {
public:
    ScenarioManager();
    ~ScenarioManager();
    
    // Scenario management
    void setScenario(ScenarioType type, const ScenarioConfig& config = ScenarioConfig());
    ScenarioType getCurrentScenario() const { return current_config_.type; }
    
    // Track generation
    std::vector<TrackState> generateInitialTracks();
    void updateTracks(std::vector<TrackState>& tracks, double delta_time_s);
    
    // Track lifecycle
    void addNewTrack(std::vector<TrackState>& tracks, uint32_t& next_id);
    void removeOutOfBoundsTracks(std::vector<TrackState>& tracks);
    bool isTrackInBounds(const TrackState& track) const;
    
    // Motion model application
    void applyMotionModel(TrackState& track, double delta_time_s);
    void applyLinearMotion(TrackState& track, double delta_time_s);
    void applyManeuveringMotion(TrackState& track, double delta_time_s);
    
    // Configuration
    ScenarioConfig getConfig() const { return current_config_; }
    void setConfig(const ScenarioConfig& config) { current_config_ = config; }

private:
    ScenarioConfig current_config_;
    uint32_t next_track_id_;
    double scenario_start_time_s_;
    
    // Random number generation
    std::mt19937 random_generator_;
    
    // Helper functions
    TrackState createRandomTrack(uint32_t id);
    double getCurrentTime() const;
    void initializeRandomGenerator();
};

} // namespace radar
} // namespace skyguardis

