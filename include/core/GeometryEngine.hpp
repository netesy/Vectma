#pragma once

#include "core/PathNode.hpp"
#include "core/Geometry.hpp"
#include <memory>
#include <vector>

namespace vectma {

class GeometryEngine {
public:
    /**
     * @brief Resolves Boolean operations on multiple selected shapes, outputting a newly formed PathNode.
     */
    static std::unique_ptr<PathNode> combinePaths(const PathNode& target, const PathNode& source, BooleanOp op);

    /**
     * @brief Flattens a contour into a sequence of points.
     */
    static std::vector<Point2D> flattenContour(const Contour& contour, double tolerance = 1.0);

    /**
     * @brief Flattens all contours in PathData.
     */
    static std::vector<std::vector<Point2D>> flattenPathData(const PathData& data, double tolerance = 1.0);

    /**
     * @brief Converts a sequence of points back into a Contour (as linear segments).
     */
    static Contour pointsToContour(const std::vector<Point2D>& points, bool closed);

private:
    // Helper to decompose complex bezier paths into linear segments for clipping
    static std::vector<Point2D> flattenPath(const PathNode& path);
};

} // namespace vectma
