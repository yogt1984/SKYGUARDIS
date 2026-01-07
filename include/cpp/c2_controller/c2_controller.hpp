#pragma once

#include "c2_controller/threat_evaluator.hpp"
#include "message_gateway/message_gateway.hpp"
#include <vector>
#include <memory>

namespace skyguardis {
namespace c2 {

class C2Controller {
public:
    C2Controller();
    ~C2Controller();
    
    void setMessageGateway(gateway::MessageGateway* gateway);
    void processTracks(const std::vector<Track>& tracks);
    void assignTarget(const Track& track);

private:
    ThreatEvaluator evaluator_;
    gateway::MessageGateway* gateway_;
};

} // namespace c2
} // namespace skyguardis

