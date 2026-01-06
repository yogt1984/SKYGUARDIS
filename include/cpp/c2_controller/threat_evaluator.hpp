#pragma once

#include <cstdint>
#include <vector>

namespace skyguardis {
namespace c2 {

struct Track {
    uint32_t id;
    double range_m;
    double azimuth_rad;
    double elevation_rad;
    double velocity_ms;
    double heading_rad;
};

class ThreatEvaluator {
public:
    struct ThreatScore {
        uint32_t track_id;
        double score;
        uint8_t priority;
    };

    ThreatScore evaluate(const Track& track) const;
    std::vector<ThreatScore> prioritize(const std::vector<Track>& tracks) const;

private:
    double computeThreatScore(const Track& track) const;
};

} // namespace c2
} // namespace skyguardis

