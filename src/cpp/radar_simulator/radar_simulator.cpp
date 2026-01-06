#include "radar_simulator/radar_simulator.hpp"
#include "c2_controller/threat_evaluator.hpp"

namespace skyguardis {
namespace radar {

class RadarSimulator {
public:
    void generateTracks();
    std::vector<Track> getCurrentTracks() const;

private:
    std::vector<Track> tracks_;
};

} // namespace radar
} // namespace skyguardis

