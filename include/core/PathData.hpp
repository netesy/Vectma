#pragma once

#include "core/GPoint.hpp"
#include <vector>

namespace vectma {

/**
 * @brief Represents a single continuous segment of a path (a contour).
 */
struct Contour {
    std::vector<BezierAnchor> anchors;
    bool isClosed = false;

    Contour() = default;
    Contour(const std::vector<BezierAnchor>& a, bool closed = false)
        : anchors(a), isClosed(closed) {}
};

/**
 * @brief Represents the raw geometric data of a path, potentially containing multiple contours.
 * Used as input/output for the modifier pipeline.
 */
struct PathData {
    std::vector<Contour> contours;

    PathData() = default;
    PathData(const std::vector<BezierAnchor>& a, bool closed = false) {
        contours.emplace_back(a, closed);
    }
};

} // namespace vectma
