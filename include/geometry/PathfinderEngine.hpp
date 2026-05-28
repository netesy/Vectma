#pragma once

#include "core/CanvasNode.hpp"
#include "core/PathNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include <memory>
#include <vector>

namespace vectma::geometry {

class PathfinderEngine {
public:
    /**
     * @brief Terminal evaluation loop that flattens a compound tree into a single PathTopology.
     * Replaces the compoundNode in its parent container with the resulting PathNode.
     */
    static void Flatten(CompoundShapeNode* compoundNode);

    /**
     * @brief Converts a node's stroke into a filled path geometry.
     * Registers a new PathNode representing the visual boundary of the stroke.
     */
    static void ConvertStrokeToPath(CanvasNode* targetNode);

    /**
     * @brief Simplifies a path using the Ramer-Douglas-Peucker algorithm for parametric curves.
     * @param node The PathNode to optimize.
     * @param epsilon The simplification threshold.
     */
    static void Optimize(PathNode* node, double epsilon = 1.0);

private:
    // Helper to extract SkPath from a CanvasNode
    static void* ExtractSkPath(const CanvasNode* node);

    // Internal RDP implementation
    static std::vector<Point2D> SimplifyPoints(const std::vector<Point2D>& points, double epsilon);
    static double PerpendicularDistance(const Point2D& pt, const Point2D& lineStart, const Point2D& lineEnd);
};

} // namespace vectma::geometry
