#pragma once

#include "core/PathNode.hpp"
#include "core/Geometry.hpp"
#include <memory>

namespace vectma {

class GeometryEngine {
public:
    /**
     * @brief Resolves Boolean operations on multiple selected shapes, outputting a newly formed PathNode.
     */
    static std::unique_ptr<PathNode> combinePaths(const PathNode& target, const PathNode& source, BooleanOp op);

private:
    // Helper to decompose complex bezier paths into linear segments for clipping
    static std::vector<Point2D> flattenPath(const PathNode& path);
};

} // namespace vectma
