#include "c2_controller/threat_evaluator.hpp"
#include <algorithm>
#include <cmath>

namespace skyguardis {
namespace c2 {

ThreatEvaluator::ThreatScore ThreatEvaluator::evaluate(const Track& track) const {
    ThreatScore score;
    score.track_id = track.id;
    score.score = computeThreatScore(track);
    score.priority = static_cast<uint8_t>(std::min(255.0, score.score * 10.0));
    return score;
}

std::vector<ThreatEvaluator::ThreatScore> ThreatEvaluator::prioritize(
    const std::vector<Track>& tracks) const {
    std::vector<ThreatScore> scores;
    for (const auto& track : tracks) {
        scores.push_back(evaluate(track));
    }
    std::sort(scores.begin(), scores.end(),
              [](const ThreatScore& a, const ThreatScore& b) {
                  return a.score > b.score;
              });
    return scores;
}

double ThreatEvaluator::computeThreatScore(const Track& track) const {
    // Simplified threat scoring: closer and faster = higher threat
    double range_factor = 1.0 / (1.0 + track.range_m / 1000.0);
    double velocity_factor = track.velocity_ms / 100.0;
    return range_factor * velocity_factor;
}

} // namespace c2
} // namespace skyguardis

