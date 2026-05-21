#pragma once

#include "core/GPoint.hpp"
#include "core/SceneGraph.hpp"
#include <vector>
#include <optional>

namespace vectma {

struct SnapResult {
    Point2D snappedPoint;
    bool snappedX = false;
    bool snappedY = false;
    std::vector<Point2D> guideLines; // Points to draw lines through
};

class SnappingEngine {
public:
    static std::optional<SnapResult> findSnapPoint(const Point2D& cursor, const SceneGraph& scene, float tolerance);
};

} // namespace vectma
