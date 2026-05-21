#pragma once

#include "core/GPoint.hpp"
#include <vector>

namespace vectma {

/**
 * @brief Represents the raw geometric data of a path.
 * Used as input/output for the modifier pipeline.
 */
struct PathData {
    std::vector<BezierAnchor> anchors;
    bool isClosed = false;

    PathData() = default;
    PathData(const std::vector<BezierAnchor>& a, bool closed = false)
        : anchors(a), isClosed(closed) {}
};

} // namespace vectma
