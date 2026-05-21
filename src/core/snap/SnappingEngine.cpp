#include "core/snap/SnappingEngine.hpp"
#include <cmath>
#include <algorithm>

namespace vectma {

std::optional<SnapResult> SnappingEngine::findSnapPoint(const Point2D& cursor, const SceneGraph& scene, float tolerance) {
    SnapResult result;
    result.snappedPoint = cursor;

    // Query spatial index for nearby nodes
    GRect queryArea(cursor.x - tolerance, cursor.y - tolerance, tolerance * 2, tolerance * 2);
    auto candidates = scene.queryVisible(queryArea);

    float bestDistX = tolerance;
    float bestDistY = tolerance;

    for (auto node : candidates) {
        GRect bbox = node->computeBoundingBox();

        // Potential snap points from bbox
        std::vector<double> snapX = { bbox.x, bbox.x + bbox.width / 2.0, bbox.x + bbox.width };
        std::vector<double> snapY = { bbox.y, bbox.y + bbox.height / 2.0, bbox.y + bbox.height };

        for (double sx : snapX) {
            float dist = std::abs(cursor.x - sx);
            if (dist < bestDistX) {
                bestDistX = dist;
                result.snappedPoint.x = sx;
                result.snappedX = true;
                // Add guide points (mock: vertical line segment)
                result.guideLines.push_back({sx, bbox.y});
                result.guideLines.push_back({sx, bbox.y + bbox.height});
            }
        }

        for (double sy : snapY) {
            float dist = std::abs(cursor.y - sy);
            if (dist < bestDistY) {
                bestDistY = dist;
                result.snappedPoint.y = sy;
                result.snappedY = true;
                // Add guide points (mock: horizontal line segment)
                result.guideLines.push_back({bbox.x, sy});
                result.guideLines.push_back({bbox.x + bbox.width, sy});
            }
        }
    }

    if (result.snappedX || result.snappedY) {
        return result;
    }

    return std::nullopt;
}

} // namespace vectma
