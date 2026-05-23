#include "core/GeometryEngine.hpp"
#include "core/PathNode.hpp"
#include <cmath>
#include <vector>
#include <algorithm>

namespace vectma {

static bool PointsAlmostEqual(const Point2D& a, const Point2D& b) {
    return std::abs(a.x - b.x) < 1e-6 && std::abs(a.y - b.y) < 1e-6;
}

std::unique_ptr<PathNode> GeometryEngine::combinePaths(const PathNode& target, const PathNode& source, BooleanOp op) {
    auto targetPath = target.getCompiledPath();
    auto sourcePath = source.getCompiledPath();

    std::vector<Contour> resultContours;

    if (op == BooleanOp::Union) {
        // High-fidelity merge: All contours from both shapes
        resultContours = targetPath.contours;
        resultContours.insert(resultContours.end(), sourcePath.contours.begin(), sourcePath.contours.end());
    } else if (op == BooleanOp::Subtract) {
        // Keep target contours that are NOT contained in source
        for (const auto& c : targetPath.contours) {
            bool contained = true;
            for (const auto& a : c.anchors) {
                if (!source.containsPoint(a.position)) { contained = false; break; }
            }
            if (!contained) resultContours.push_back(c);
        }
    } else if (op == BooleanOp::Intersect) {
        // Keep target contours that ARE contained in source
        for (const auto& c : targetPath.contours) {
            bool contained = false;
            for (const auto& a : c.anchors) {
                if (source.containsPoint(a.position)) { contained = true; break; }
            }
            if (contained) resultContours.push_back(c);
        }
        // Also keep source contours that are contained in target
        for (const auto& c : sourcePath.contours) {
            bool contained = false;
            for (const auto& a : c.anchors) {
                if (target.containsPoint(a.position)) { contained = true; break; }
            }
            if (contained) resultContours.push_back(c);
        }
    } else if (op == BooleanOp::Exclude) {
        // Keep contours from both that are NOT contained in the other (XOR)
        for (const auto& c : targetPath.contours) {
            bool contained = true;
            for (const auto& a : c.anchors) {
                if (!source.containsPoint(a.position)) { contained = false; break; }
            }
            if (!contained) resultContours.push_back(c);
        }
        for (const auto& c : sourcePath.contours) {
            bool contained = true;
            for (const auto& a : c.anchors) {
                if (!target.containsPoint(a.position)) { contained = false; break; }
            }
            if (!contained) resultContours.push_back(c);
        }
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

        bool isLinear = PointsAlmostEqual(p0, p1) && PointsAlmostEqual(p2, p3);

        if (isLinear) {
            points.push_back(p0);
        } else {
            constexpr int steps = 32;
            for (int t_step = 0; t_step < steps; ++t_step) {
                double t = t_step / (double)steps;
                double t1 = 1.0 - t;
                double x = t1*t1*t1*p0.x + 3*t1*t1*t*p1.x + 3*t1*t*t*p2.x + t*t*t*p3.x;
                double y = t1*t1*t1*p0.y + 3*t1*t1*t*p1.y + 3*t1*t*t*p2.y + t*t*t*p3.y;
                points.emplace_back(x, y);
            }
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
    auto compiled = path.getCompiledPath();
    if (compiled.contours.empty()) return {};
    return flattenContour(compiled.contours[0]);
}

} // namespace vectma
