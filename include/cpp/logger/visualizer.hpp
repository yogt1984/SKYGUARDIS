#pragma once

#include "c2_controller/threat_evaluator.hpp"
#include <vector>

namespace skyguardis {
namespace logger {

class Visualizer {
public:
    void visualize(const std::vector<c2::Track>& tracks);
};

} // namespace logger
} // namespace skyguardis

