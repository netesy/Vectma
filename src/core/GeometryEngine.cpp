#include "core/GeometryEngine.hpp"
#include <cmath>
#include <vector>

namespace vectma {

std::unique_ptr<PathNode> GeometryEngine::combinePaths(const PathNode& target, const PathNode& source, BooleanOp op) {
    auto targetPts = flattenPath(target);
    auto sourcePts = flattenPath(source);

    std::vector<BezierAnchor> resultAnchors;

    if (op == BooleanOp::Union) {
        for (const auto& a : target.getAnchors()) resultAnchors.push_back(a);
        for (const auto& a : source.getAnchors()) resultAnchors.push_back(a);
    } else if (op == BooleanOp::Intersect) {
        auto b1 = target.computeBoundingBox();
        auto b2 = source.computeBoundingBox();

        double ix = std::max(b1.x, b2.x);
        double iy = std::max(b1.y, b2.y);
        double iw = std::min(b1.x + b1.width, b2.x + b2.width) - ix;
        double ih = std::min(b1.y + b1.height, b2.y + b2.height) - iy;

        if (iw > 0 && ih > 0) {
            resultAnchors.emplace_back(Point2D(ix, iy), Point2D(ix, iy), Point2D(ix, iy));
            resultAnchors.emplace_back(Point2D(ix+iw, iy+ih), Point2D(ix+iw, iy+ih), Point2D(ix+iw, iy+ih));
        }
    }

    return std::make_unique<PathNode>(resultAnchors);
}

std::vector<Point2D> GeometryEngine::flattenPath(const PathNode& path) {
    std::vector<Point2D> points;
    const auto& anchors = path.getAnchors();
    if (anchors.empty()) return points;

    for (size_t i = 0; i < anchors.size() - 1; ++i) {
        const auto& p0 = anchors[i].position;
        const auto& p1 = anchors[i].handleOut;
        const auto& p2 = anchors[i+1].handleIn;
        const auto& p3 = anchors[i+1].position;

        for (int t_step = 0; t_step <= 10; ++t_step) {
            double t = t_step / 10.0;
            double t1 = 1.0 - t;
            double x = t1*t1*t1*p0.x + 3*t1*t1*t*p1.x + 3*t1*t*t*p2.x + t*t*t*p3.x;
            double y = t1*t1*t1*p0.y + 3*t1*t1*t*p1.y + 3*t1*t*t*p2.y + t*t*t*p3.y;
            points.emplace_back(x, y);
        }
    }
    return points;
}

} // namespace vectma
