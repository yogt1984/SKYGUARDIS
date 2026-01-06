#pragma once

#include "c2_controller/threat_evaluator.hpp"
#include <vector>

namespace skyguardis {
namespace radar {

class RadarSimulator {
public:
    void generateTracks();
    std::vector<c2::Track> getCurrentTracks() const;

private:
    std::vector<c2::Track> tracks_;
};

} // namespace radar
} // namespace skyguardis

