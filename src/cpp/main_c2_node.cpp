#include "c2_controller/c2_controller.hpp"
#include "radar_simulator/radar_simulator.hpp"
#include "message_gateway/message_gateway.hpp"
#include "logger/logger.hpp"
#include <iostream>

int main() {
    std::cout << "SKYGUARDIS C2 Node starting..." << std::endl;
    
    // Initialize components
    skyguardis::radar::RadarSimulator radar;
    skyguardis::c2::C2Controller c2;
    skyguardis::gateway::MessageGateway gateway;
    skyguardis::logger::Logger logger;
    
    // Main control loop would go here
    logger.log("C2 Node initialized");
    
    return 0;
}

