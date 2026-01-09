#pragma once

#include "c2_controller/threat_evaluator.hpp"
#include "message_gateway/protocol.hpp"
#include <vector>
#include <string>
#include <memory>
#include <chrono>

namespace skyguardis {
namespace logger {

// Visualization output format
enum class VisualFormat {
    ASCII_TABLE,    // Tabular ASCII output
    ASCII_ART,      // Simple ASCII art visualization
    COMPACT         // Compact single-line format
};

class Visualizer {
public:
    Visualizer();
    ~Visualizer();
    
    // Track visualization
    void visualizeTracks(const std::vector<c2::Track>& tracks);
    void visualizeTracksTable(const std::vector<c2::Track>& tracks);
    void visualizeTracksCompact(const std::vector<c2::Track>& tracks);
    
    // Engagement status visualization
    void visualizeEngagementStatus(const protocol::EngagementStatus& status);
    void visualizeEngagementStatusDetailed(const protocol::EngagementStatus& status);
    
    // Safety status visualization
    void visualizeSafetyStatus(bool is_safe, const std::string& reason = "");
    void visualizeSafetyViolation(const std::string& violation_type, const std::string& details);
    
    // Combined dashboard
    void visualizeDashboard(
        const std::vector<c2::Track>& tracks,
        const protocol::EngagementStatus& status,
        bool safety_status
    );
    
    // Configuration
    void setFormat(VisualFormat format);
    void setUpdateInterval(int cycles); // Update every N cycles
    void enableAutoClear(bool enable);  // Clear screen before update
    void setOutputFile(const std::string& filename);
    
    // Legacy method (for compatibility)
    void visualize(const std::vector<c2::Track>& tracks);

private:
    VisualFormat format_;
    int update_interval_;
    int cycle_count_;
    bool auto_clear_;
    std::unique_ptr<std::ofstream> output_file_;
    
    // Helper methods
    void clearScreen();
    void printHeader();
    void printSeparator();
    std::string formatAngle(double radians) const;
    std::string formatRange(double meters) const;
    std::string formatVelocity(double ms) const;
    std::string getStateString(uint8_t state) const;
    std::string getSafetyStatusString(bool is_safe) const;
    void writeToFile(const std::string& content);
};

} // namespace logger
} // namespace skyguardis
