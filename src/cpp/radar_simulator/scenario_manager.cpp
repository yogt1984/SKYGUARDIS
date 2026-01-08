#include "radar_simulator/scenario_manager.hpp"
#include <random>
#include <cmath>
#include <algorithm>
#include <chrono>

namespace skyguardis {
namespace radar {

ScenarioManager::ScenarioManager() : next_track_id_(1), scenario_start_time_s_(0.0) {
    initializeRandomGenerator();
    scenario_start_time_s_ = getCurrentTime();
}

ScenarioManager::~ScenarioManager() {}

void ScenarioManager::initializeRandomGenerator() {
    std::random_device rd;
    random_generator_.seed(rd());
}

double ScenarioManager::getCurrentTime() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration<double>(duration).count();
}

void ScenarioManager::setScenario(ScenarioType type, const ScenarioConfig& config) {
    current_config_ = config;
    current_config_.type = type;
    
    // Adjust target count based on scenario type
    switch (type) {
        case ScenarioType::SINGLE_TARGET:
            current_config_.target_count = 1;
            break;
        case ScenarioType::SWARM:
            if (current_config_.target_count < 3) current_config_.target_count = 3;
            if (current_config_.target_count > 10) current_config_.target_count = 10;
            break;
        case ScenarioType::SATURATION:
            if (current_config_.target_count < 10) current_config_.target_count = 10;
            if (current_config_.target_count > 20) current_config_.target_count = 20;
            break;
    }
    
    scenario_start_time_s_ = getCurrentTime();
    next_track_id_ = 1;
}

std::vector<TrackState> ScenarioManager::generateInitialTracks() {
    std::vector<TrackState> tracks;
    
    for (uint32_t i = 0; i < current_config_.target_count; ++i) {
        TrackState track = createRandomTrack(next_track_id_++);
        track.entry_time_s = getCurrentTime();
        track.last_update_time_s = track.entry_time_s;
        tracks.push_back(track);
    }
    
    return tracks;
}

TrackState ScenarioManager::createRandomTrack(uint32_t id) {
    TrackState track;
    track.track.id = id;
    
    // Random position within bounds
    std::uniform_real_distribution<double> range_dist(
        current_config_.min_range_m, current_config_.max_range_m);
    std::uniform_real_distribution<double> azimuth_dist(-3.14159, 3.14159);
    std::uniform_real_distribution<double> elevation_dist(
        current_config_.min_elevation_rad, current_config_.max_elevation_rad);
    std::uniform_real_distribution<double> velocity_dist(
        current_config_.min_velocity_ms, current_config_.max_velocity_ms);
    std::uniform_real_distribution<double> heading_dist(-3.14159, 3.14159);
    
    track.track.range_m = range_dist(random_generator_);
    track.track.azimuth_rad = azimuth_dist(random_generator_);
    track.track.elevation_rad = elevation_dist(random_generator_);
    track.track.velocity_ms = velocity_dist(random_generator_);
    track.track.heading_rad = heading_dist(random_generator_);
    
    // Random motion model
    std::uniform_int_distribution<int> model_dist(0, 1);
    track.motion_model = (model_dist(random_generator_) == 0) ? 
                        MotionModel::LINEAR : MotionModel::MANEUVERING;
    
    // Set motion parameters
    if (track.motion_model == MotionModel::MANEUVERING) {
        std::uniform_real_distribution<double> accel_dist(-50.0, 50.0);
        std::uniform_real_distribution<double> angular_dist(-0.5, 0.5);
        track.acceleration_ms2 = accel_dist(random_generator_);
        track.angular_velocity_rads = angular_dist(random_generator_);
    } else {
        track.acceleration_ms2 = 0.0;
        track.angular_velocity_rads = 0.0;
    }
    
    track.active = true;
    track.age_cycles = 0;
    
    return track;
}

void ScenarioManager::updateTracks(std::vector<TrackState>& tracks, double delta_time_s) {
    double current_time = getCurrentTime();
    
    // Update existing tracks
    for (auto& track : tracks) {
        if (!track.active) continue;
        
        // Apply motion model
        applyMotionModel(track, delta_time_s);
        
        // Update history
        if (track.history.size() >= TrackState::MAX_HISTORY) {
            track.history.erase(track.history.begin());
        }
        track.history.push_back(track.track);
        
        // Update metadata
        track.last_update_time_s = current_time;
        track.age_cycles++;
    }
    
    // Remove out-of-bounds tracks
    removeOutOfBoundsTracks(tracks);
    
    // Add new tracks based on scenario type
    switch (current_config_.type) {
        case ScenarioType::SWARM:
        case ScenarioType::SATURATION:
            // Periodically add new tracks
            if (tracks.size() < current_config_.target_count) {
                std::uniform_real_distribution<double> add_prob(0.0, 1.0);
                if (add_prob(random_generator_) < 0.1) { // 10% chance per cycle
                    addNewTrack(tracks, next_track_id_);
                }
            }
            break;
        case ScenarioType::SINGLE_TARGET:
            // Only add if no tracks exist
            if (tracks.empty()) {
                addNewTrack(tracks, next_track_id_);
            }
            break;
    }
}

void ScenarioManager::applyMotionModel(TrackState& track, double delta_time_s) {
    switch (track.motion_model) {
        case MotionModel::LINEAR:
            applyLinearMotion(track, delta_time_s);
            break;
        case MotionModel::MANEUVERING:
            applyManeuveringMotion(track, delta_time_s);
            break;
    }
}

void ScenarioManager::applyLinearMotion(TrackState& track, double delta_time_s) {
    // Constant velocity motion
    // Update position based on velocity and heading
    double distance = track.track.velocity_ms * delta_time_s;
    
    // Convert spherical to Cartesian for motion, then back to spherical
    // Simplified: update range and angles based on velocity direction
    double range_change = distance * std::cos(track.track.elevation_rad) * 
                         std::cos(track.track.heading_rad);
    double azimuth_change = distance * std::cos(track.track.elevation_rad) * 
                           std::sin(track.track.heading_rad) / track.track.range_m;
    double elevation_change = -distance * std::sin(track.track.elevation_rad) / 
                             track.track.range_m;
    
    track.track.range_m += range_change;
    track.track.azimuth_rad += azimuth_change;
    track.track.elevation_rad += elevation_change;
    
    // Normalize angles
    while (track.track.azimuth_rad > 3.14159) track.track.azimuth_rad -= 2.0 * 3.14159;
    while (track.track.azimuth_rad < -3.14159) track.track.azimuth_rad += 2.0 * 3.14159;
    track.track.elevation_rad = std::max(-1.5708, std::min(1.5708, track.track.elevation_rad));
}

void ScenarioManager::applyManeuveringMotion(TrackState& track, double delta_time_s) {
    // Velocity changes with acceleration
    track.track.velocity_ms += track.acceleration_ms2 * delta_time_s;
    
    // Apply velocity constraints
    track.track.velocity_ms = std::max(50.0, std::min(500.0, track.track.velocity_ms));
    
    // Update heading with angular velocity
    track.track.heading_rad += track.angular_velocity_rads * delta_time_s;
    while (track.track.heading_rad > 3.14159) track.track.heading_rad -= 2.0 * 3.14159;
    while (track.track.heading_rad < -3.14159) track.track.heading_rad += 2.0 * 3.14159;
    
    // Apply linear motion with updated velocity
    applyLinearMotion(track, delta_time_s);
    
    // Randomly change acceleration/angular velocity (simulate maneuvering)
    std::uniform_real_distribution<double> change_prob(0.0, 1.0);
    if (change_prob(random_generator_) < 0.05) { // 5% chance per cycle
        std::uniform_real_distribution<double> accel_dist(-50.0, 50.0);
        std::uniform_real_distribution<double> angular_dist(-0.5, 0.5);
        track.acceleration_ms2 = accel_dist(random_generator_);
        track.angular_velocity_rads = angular_dist(random_generator_);
    }
}

void ScenarioManager::addNewTrack(std::vector<TrackState>& tracks, uint32_t& next_id) {
    TrackState new_track = createRandomTrack(next_id++);
    new_track.entry_time_s = getCurrentTime();
    new_track.last_update_time_s = new_track.entry_time_s;
    tracks.push_back(new_track);
}

void ScenarioManager::removeOutOfBoundsTracks(std::vector<TrackState>& tracks) {
    tracks.erase(
        std::remove_if(tracks.begin(), tracks.end(),
            [this](const TrackState& track) {
                return !isTrackInBounds(track);
            }),
        tracks.end()
    );
}

bool ScenarioManager::isTrackInBounds(const TrackState& track) const {
    // Check if track is within detection zone
    if (track.track.range_m < current_config_.min_range_m ||
        track.track.range_m > current_config_.detection_zone_radius_m) {
        return false;
    }
    
    if (track.track.elevation_rad < current_config_.min_elevation_rad ||
        track.track.elevation_rad > current_config_.max_elevation_rad) {
        return false;
    }
    
    return true;
}

} // namespace radar
} // namespace skyguardis

