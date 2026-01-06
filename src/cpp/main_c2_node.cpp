#include "c2_controller/c2_controller.hpp"
#include "radar_simulator/radar_simulator.hpp"
#include "message_gateway/message_gateway.hpp"
#include "logger/logger.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>
#include <atomic>

std::atomic<bool> running(true);

void signalHandler(int signal) {
    running = false;
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    std::cout << "[C2_NODE] SKYGUARDIS C2 Node starting..." << std::endl;
    
    // Initialize components
    skyguardis::radar::RadarSimulator radar;
    skyguardis::c2::C2Controller c2;
    skyguardis::logger::Logger logger;
    // skyguardis::gateway::MessageGateway gateway;  // Reserved for future use
    
    logger.log("C2 Node initialized");
    
    // Main control loop
    int cycle = 0;
    while (running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Generate and process tracks
        radar.generateTracks();
        auto tracks = radar.getCurrentTracks();
        
        if (!tracks.empty()) {
            c2.processTracks(tracks);
            logger.log("Cycle " + std::to_string(cycle) + ": Processed " + 
                      std::to_string(tracks.size()) + " tracks");
        }
        
        cycle++;
        if (cycle % 100 == 0) {
            logger.log("C2 Node running - cycle " + std::to_string(cycle));
        }
    }
    
    logger.log("C2 Node shutting down");
    std::cout << "[C2_NODE] Shutdown complete" << std::endl;
    return 0;
}

