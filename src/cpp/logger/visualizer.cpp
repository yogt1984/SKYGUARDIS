#include "logger/visualizer.hpp"
#include "c2_controller/threat_evaluator.hpp"
#include "message_gateway/protocol.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <fstream>
#include <algorithm>

namespace skyguardis {
namespace logger {

Visualizer::Visualizer() 
    : format_(VisualFormat::ASCII_TABLE),
      update_interval_(10),  // Update every 10 cycles (1 second at 10 Hz)
      cycle_count_(0),
      auto_clear_(false) {}

Visualizer::~Visualizer() {
    if (output_file_ && output_file_->is_open()) {
        output_file_->close();
    }
}

void Visualizer::setFormat(VisualFormat format) {
    format_ = format;
}

void Visualizer::setUpdateInterval(int cycles) {
    update_interval_ = std::max(1, cycles);
}

void Visualizer::enableAutoClear(bool enable) {
    auto_clear_ = enable;
}

void Visualizer::setOutputFile(const std::string& filename) {
    if (output_file_ && output_file_->is_open()) {
        output_file_->close();
    }
    output_file_ = std::make_unique<std::ofstream>(filename, std::ios::app);
}

void Visualizer::clearScreen() {
    if (auto_clear_) {
        std::cout << "\033[2J\033[H"; // ANSI escape codes for clear screen
    }
}

void Visualizer::printHeader() {
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              SKYGUARDIS Air Defence System Status             ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
}

void Visualizer::printSeparator() {
    std::cout << "────────────────────────────────────────────────────────────────\n";
}

std::string Visualizer::formatAngle(double radians) const {
    double degrees = radians * 180.0 / 3.14159265359;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << degrees << "°";
    return ss.str();
}

std::string Visualizer::formatRange(double meters) const {
    std::stringstream ss;
    if (meters >= 1000.0) {
        ss << std::fixed << std::setprecision(1) << (meters / 1000.0) << " km";
    } else {
        ss << std::fixed << std::setprecision(0) << meters << " m";
    }
    return ss.str();
}

std::string Visualizer::formatVelocity(double ms) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << ms << " m/s";
    return ss.str();
}

std::string Visualizer::getStateString(uint8_t state) const {
    switch (state) {
        case 0: return "IDLE";
        case 1: return "ACQUIRING";
        case 2: return "TRACKING";
        case 3: return "FIRING";
        case 4: return "VERIFYING";
        case 5: return "COMPLETE";
        default: return "UNKNOWN";
    }
}

std::string Visualizer::getSafetyStatusString(bool is_safe) const {
    return is_safe ? "✓ SAFE" : "✗ UNSAFE";
}

void Visualizer::writeToFile(const std::string& content) {
    if (output_file_ && output_file_->is_open()) {
        *output_file_ << content;
        output_file_->flush();
    }
}

void Visualizer::visualizeTracks(const std::vector<c2::Track>& tracks) {
    cycle_count_++;
    if (cycle_count_ % update_interval_ != 0) {
        return; // Skip this cycle
    }
    
    switch (format_) {
        case VisualFormat::ASCII_TABLE:
            visualizeTracksTable(tracks);
            break;
        case VisualFormat::COMPACT:
            visualizeTracksCompact(tracks);
            break;
        case VisualFormat::ASCII_ART:
            visualizeTracksTable(tracks); // Use table for ASCII_ART too
            break;
    }
}

void Visualizer::visualizeTracksTable(const std::vector<c2::Track>& tracks) {
    clearScreen();
    printHeader();
    
    std::cout << "\n📡 ACTIVE TRACKS: " << tracks.size() << "\n";
    printSeparator();
    
    if (tracks.empty()) {
        std::cout << "  No active tracks\n";
    } else {
        std::cout << std::left 
                  << std::setw(8) << "ID"
                  << std::setw(12) << "Range"
                  << std::setw(12) << "Azimuth"
                  << std::setw(12) << "Elevation"
                  << std::setw(12) << "Velocity"
                  << std::setw(10) << "Heading"
                  << "\n";
        printSeparator();
        
        for (const auto& track : tracks) {
            std::cout << std::left
                      << std::setw(8) << track.id
                      << std::setw(12) << formatRange(track.range_m)
                      << std::setw(12) << formatAngle(track.azimuth_rad)
                      << std::setw(12) << formatAngle(track.elevation_rad)
                      << std::setw(12) << formatVelocity(track.velocity_ms)
                      << std::setw(10) << formatAngle(track.heading_rad)
                      << "\n";
        }
    }
    
    printSeparator();
    
    // Write to file if configured
    std::stringstream ss;
    ss << "Tracks: " << tracks.size() << "\n";
    writeToFile(ss.str());
}

void Visualizer::visualizeTracksCompact(const std::vector<c2::Track>& tracks) {
    std::cout << "[TRACKS: " << tracks.size();
    if (!tracks.empty()) {
        std::cout << " | ID:" << tracks[0].id 
                  << " R:" << formatRange(tracks[0].range_m)
                  << " V:" << formatVelocity(tracks[0].velocity_ms);
    }
    std::cout << "]\n";
}

void Visualizer::visualizeEngagementStatus(const protocol::EngagementStatus& status) {
    cycle_count_++;
    if (cycle_count_ % update_interval_ != 0) {
        return;
    }
    
    visualizeEngagementStatusDetailed(status);
}

void Visualizer::visualizeEngagementStatusDetailed(const protocol::EngagementStatus& status) {
    std::cout << "\n🎯 ENGAGEMENT STATUS\n";
    printSeparator();
    std::cout << "  Target ID:      " << status.target_id << "\n";
    std::cout << "  State:          " << getStateString(status.state) << "\n";
    std::cout << "  Firing:         " << (status.firing ? "YES" : "NO") << "\n";
    std::cout << "  Lead Angle:     " << formatAngle(status.lead_angle_rad) << "\n";
    std::cout << "  Time to Impact: " << std::fixed << std::setprecision(2) 
              << status.time_to_impact_s << " s\n";
    printSeparator();
    
    // Write to file
    std::stringstream ss;
    ss << "Engagement: Target=" << status.target_id 
       << " State=" << getStateString(status.state)
       << " Firing=" << (status.firing ? "YES" : "NO") << "\n";
    writeToFile(ss.str());
}

void Visualizer::visualizeSafetyStatus(bool is_safe, const std::string& reason) {
    std::cout << "\n🛡️  SAFETY STATUS: " << getSafetyStatusString(is_safe);
    if (!reason.empty()) {
        std::cout << " (" << reason << ")";
    }
    std::cout << "\n";
    
    // Write to file
    std::stringstream ss;
    ss << "Safety: " << (is_safe ? "SAFE" : "UNSAFE");
    if (!reason.empty()) {
        ss << " - " << reason;
    }
    ss << "\n";
    writeToFile(ss.str());
}

void Visualizer::visualizeSafetyViolation(const std::string& violation_type, const std::string& details) {
    std::cout << "\n⚠️  SAFETY VIOLATION DETECTED\n";
    printSeparator();
    std::cout << "  Type:    " << violation_type << "\n";
    std::cout << "  Details: " << details << "\n";
    printSeparator();
    
    // Write to file
    std::stringstream ss;
    ss << "SAFETY VIOLATION: " << violation_type << " - " << details << "\n";
    writeToFile(ss.str());
}

void Visualizer::visualizeDashboard(
    const std::vector<c2::Track>& tracks,
    const protocol::EngagementStatus& status,
    bool safety_status
) {
    cycle_count_++;
    if (cycle_count_ % update_interval_ != 0) {
        return;
    }
    
    clearScreen();
    printHeader();
    
    // Tracks section
    visualizeTracksTable(tracks);
    
    // Engagement status section
    visualizeEngagementStatusDetailed(status);
    
    // Safety status section
    visualizeSafetyStatus(safety_status);
    
    std::cout << "\n";
}

void Visualizer::visualize(const std::vector<c2::Track>& tracks) {
    visualizeTracks(tracks);
}

} // namespace logger
} // namespace skyguardis
