#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <optional>

namespace vectma {

struct Waypoint {
    std::string name;
    double centerX;
    double centerY;
    double zoomFactor;
};

class WaypointManager {
public:
    WaypointManager();
    ~WaypointManager() = default;

    void saveWaypoint(int slot, const std::string& name, double centerX, double centerY, double zoomFactor);
    std::optional<Waypoint> getWaypoint(int slot) const;
    void clearWaypoint(int slot);

    static constexpr int MAX_WAYPOINTS = 9;

private:
    mutable std::mutex m_mutex;
    std::vector<std::optional<Waypoint>> m_waypoints;
};

} // namespace vectma
