#include "c2_controller/c2_controller.hpp"
#include "c2_controller/threat_evaluator.hpp"
#include "message_gateway/protocol.hpp"

namespace skyguardis {
namespace c2 {

class C2Controller {
public:
    void processTracks(const std::vector<Track>& tracks);
    void assignTarget(const Track& track);

private:
    ThreatEvaluator evaluator_;
};

} // namespace c2
} // namespace skyguardis

