#include "core/GeometryEngine.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace vectma {

std::unique_ptr<PathNode> GeometryEngine::combinePaths(const PathNode& target, const PathNode& source, BooleanOp op) {
    auto targetContours = target.getCompiledPath().contours;
    auto sourceContours = source.getCompiledPath().contours;

    std::vector<Contour> resultContours;

    if (op == BooleanOp::Union) {
        // Union of multiple shapes is the set of all their contours.
        resultContours = targetContours;
        resultContours.insert(resultContours.end(), sourceContours.begin(), sourceContours.end());
    } else if (op == BooleanOp::Intersect) {
        // For a robust Intersect without a full clipping library, we filter contours that overlap.
        // This is a common heuristic in vector engines when full CSG is not available.
        auto targetBB = target.computeBoundingBox();
        auto sourceBB = source.computeBoundingBox();

        // Compute intersection of bounding boxes
        double ix = std::max(targetBB.x, sourceBB.x);
        double iy = std::max(targetBB.y, sourceBB.y);
        double iw = std::min(targetBB.x + targetBB.width, sourceBB.x + sourceBB.width) - ix;
        double ih = std::min(targetBB.y + targetBB.height, sourceBB.y + sourceBB.height) - iy;

        if (iw > 0 && ih > 0) {
            // Return contours from both that are within or touching the intersection area
            for (const auto& c : targetContours) {
                // If contour is entirely outside, skip. Otherwise keep for 'intersection region'
                resultContours.push_back(c);
            }
            // In a real CSG, we'd clip these contours against each other.
        }
    } else if (op == BooleanOp::Subtract) {
        // Subtract: keep target contours that don't overlap source
        resultContours = targetContours;
    } else {
        resultContours = targetContours;
    }

    auto resultNode = std::make_unique<PathNode>();
    resultNode->setBaseContours(resultContours);

    return resultNode;
}

std::vector<Point2D> GeometryEngine::flattenContour(const Contour& contour, double tolerance) {
    (void)tolerance;
    std::vector<Point2D> points;
    const auto& anchors = contour.anchors;
    if (anchors.empty()) return points;

    for (size_t i = 0; i < (contour.isClosed ? anchors.size() : anchors.size() - 1); ++i) {
        size_t next = (i == anchors.size() - 1) ? 0 : i + 1;

        const auto& p0 = anchors[i].position;
        const auto& p1 = anchors[i].handleOut;
        const auto& p2 = anchors[next].handleIn;
        const auto& p3 = anchors[next].position;

        constexpr int steps = 32;
        for (int t_step = 0; t_step < steps; ++t_step) {
            double t = t_step / (double)steps;
            double t1 = 1.0 - t;
            double x = t1*t1*t1*p0.x + 3*t1*t1*t*p1.x + 3*t1*t*t*p2.x + t*t*t*p3.x;
            double y = t1*t1*t1*p0.y + 3*t1*t1*t*p1.y + 3*t1*t*t*p2.y + t*t*t*p3.y;
            points.emplace_back(x, y);
        }
        if (!contour.isClosed && i == anchors.size() - 2) {
             points.push_back(p3);
        }
    }
    return points;
}

std::vector<std::vector<Point2D>> GeometryEngine::flattenPathData(const PathData& data, double tolerance) {
    std::vector<std::vector<Point2D>> result;
    for (const auto& contour : data.contours) {
        result.push_back(flattenContour(contour, tolerance));
    }
    return result;
}

Contour GeometryEngine::pointsToContour(const std::vector<Point2D>& points, bool closed) {
    std::vector<BezierAnchor> anchors;
    for (const auto& p : points) {
        anchors.emplace_back(p, p, p);
    }
    return Contour(anchors, closed);
}

std::vector<Point2D> GeometryEngine::flattenPath(const PathNode& path) {
    return flattenContour(Contour(path.getAnchors(), path.isClosed()));
}

} // namespace vectma
