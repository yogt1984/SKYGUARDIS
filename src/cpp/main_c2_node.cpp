#include "c2_controller/c2_controller.hpp"
#include "radar_simulator/radar_simulator.hpp"
#include "message_gateway/message_gateway.hpp"
#include "logger/logger.hpp"
#include "logger/visualizer.hpp"
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
    skyguardis::logger::Visualizer visualizer;
    skyguardis::gateway::MessageGateway gateway;
    
    // Initialize message gateway
    if (!gateway.initialize(8888, 8889)) {
        logger.error("Failed to initialize message gateway");
        std::cerr << "[C2_NODE] Failed to initialize message gateway" << std::endl;
        return 1;
    }
    logger.info("Message gateway initialized successfully");
    
    // Connect gateway to C2 controller
    c2.setMessageGateway(&gateway);
    
    logger.info("C2 Node initialized");
    logger.setLogFile("logs/c2_node.log");
    
    // Configure visualizer
    visualizer.setFormat(skyguardis::logger::VisualFormat::ASCII_TABLE);
    visualizer.setUpdateInterval(10); // Update every 1 second (10 cycles)
    visualizer.enableAutoClear(false); // Don't clear screen (for log files)
    visualizer.setOutputFile("logs/visualization.log");
    
    // Performance monitoring
    auto cycle_start_time = std::chrono::steady_clock::now();
    int cycle = 0;
    double total_cycle_time = 0.0;
    
    // Main control loop
    while (running) {
        auto cycle_begin = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        try {
            // Update and process tracks (with motion models)
            radar.updateTracks();
            auto tracks = radar.getCurrentTracks();
            
            if (!tracks.empty()) {
                c2.processTracks(tracks);
                logger.debug("Cycle " + std::to_string(cycle) + ": Processed " + 
                          std::to_string(tracks.size()) + " tracks");
                
                // Log target assignments
                for (const auto& track : tracks) {
                    logger.logTargetAssignment(track.id, track.range_m, track.azimuth_rad);
                }
            }
            
            // Check for engagement status updates
            skyguardis::protocol::EngagementStatus status;
            bool has_status = gateway.receiveEngagementStatus(status);
            bool safety_status = true; // Default safe
            
            if (has_status) {
                logger.logEngagement(status);
                logger.logStateTransition("Previous", "State_" + std::to_string(status.state));
                
                // Determine safety status from engagement state
                if (status.state == 0) { // Idle
                    safety_status = true;
                }
            }
            
            // Visualize dashboard (tracks + engagement status if available)
            if (has_status) {
                visualizer.visualizeDashboard(tracks, status, safety_status);
            } else {
                // Visualize tracks only if no engagement status
                visualizer.visualizeTracks(tracks);
            }
            
            // Performance monitoring
            auto cycle_end = std::chrono::steady_clock::now();
            auto cycle_duration = std::chrono::duration_cast<std::chrono::microseconds>(
                cycle_end - cycle_begin).count() / 1000.0; // Convert to ms
            total_cycle_time += cycle_duration;
            
            cycle++;
            if (cycle % 100 == 0) {
                double avg_cycle_time = total_cycle_time / 100.0;
                logger.info("C2 Node running - cycle " + std::to_string(cycle));
                logger.logPerformanceMetric("avg_cycle_time", avg_cycle_time, "ms");
                logger.logPerformanceMetric("active_tracks", static_cast<double>(tracks.size()));
                total_cycle_time = 0.0;
            }
        } catch (const std::exception& e) {
            logger.error("C2 Node error: " + std::string(e.what()));
            // Continue running, don't crash
        } catch (...) {
            logger.error("C2 Node unknown error occurred");
        }
    }
    
    gateway.shutdown();
    logger.info("C2 Node shutting down gracefully");
    std::cout << "[C2_NODE] Shutdown complete" << std::endl;
    return 0;
}

