#include "core/WaypointManager.hpp"

namespace vectma {

WaypointManager::WaypointManager() {
    m_waypoints.resize(MAX_WAYPOINTS, std::nullopt);
}

void WaypointManager::saveWaypoint(int slot, const std::string& name, double centerX, double centerY, double zoomFactor) {
    if (slot < 0 || slot >= MAX_WAYPOINTS) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_waypoints[slot] = Waypoint{name, centerX, centerY, zoomFactor};
}

std::optional<Waypoint> WaypointManager::getWaypoint(int slot) const {
    if (slot < 0 || slot >= MAX_WAYPOINTS) return std::nullopt;
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_waypoints[slot];
}

void WaypointManager::clearWaypoint(int slot) {
    if (slot < 0 || slot >= MAX_WAYPOINTS) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    m_waypoints[slot] = std::nullopt;
}

} // namespace vectma
