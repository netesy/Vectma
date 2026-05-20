#include "core/PathNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>
#include <algorithm>

namespace vectma {

std::vector<BezierAnchor> evaluateRoundedPath(const std::vector<BezierAnchor>& baseAnchors, float radius) {
    if (radius <= 0.0f || baseAnchors.size() < 2) return baseAnchors;
    return baseAnchors;
}

PathNode::PathNode() = default;

PathNode::PathNode(const std::vector<BezierAnchor>& anchors)
    : m_anchors(anchors) {}

void PathNode::render(RenderPipeline& pipeline) const {
    auto rounded = evaluateRoundedPath(m_anchors, cornerRadius);
    (void)rounded;
    pipeline.setStrokeStyle(dashPattern, strokeOffset);
    pipeline.drawPath(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool PathNode::containsPoint(const GPoint& point) const {
    (void)point;
    return hitTestAnchors(point, 5.0f) != -1;
}

GRect PathNode::computeBoundingBox() const {
    if (m_anchors.empty()) return GRect(0, 0, 0, 0);

    double minX = m_anchors[0].position.x;
    double minY = m_anchors[0].position.y;
    double maxX = minX;
    double maxY = minY;

    auto update = [&](const Point2D& p) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
    };

    for (const auto& anchor : m_anchors) {
        update(anchor.position);
        update(anchor.handleIn);
        update(anchor.handleOut);
    }

    double x = minX;
    double y = minY;
    double w = maxX - minX;
    double h = maxY - minY;

    double halfStroke = getStrokeWidth() / 2.0;
    if (getStrokeAlignment() == StrokeAlignment::Center) {
        x -= halfStroke;
        y -= halfStroke;
        w += getStrokeWidth();
        h += getStrokeWidth();
    } else if (getStrokeAlignment() == StrokeAlignment::Outside) {
        x -= getStrokeWidth();
        y -= getStrokeWidth();
        w += getStrokeWidth() * 2.0;
        h += getStrokeWidth() * 2.0;
    }

    return GRect(x, y, w, h);
}

int PathNode::hitTestAnchors(const Point2D& canvasPos, float toleranceRadius) const {
    float tolSq = toleranceRadius * toleranceRadius;

    auto distSq = [](const Point2D& p1, const Point2D& p2) {
        double dx = p1.x - p2.x;
        double dy = p1.y - p2.y;
        return (float)(dx * dx + dy * dy);
    };

    for (size_t i = 0; i < m_anchors.size(); ++i) {
        if (distSq(canvasPos, m_anchors[i].position) <= tolSq) return (int)(i << 2) | 0;
        if (distSq(canvasPos, m_anchors[i].handleIn) <= tolSq) return (int)(i << 2) | 1;
        if (distSq(canvasPos, m_anchors[i].handleOut) <= tolSq) return (int)(i << 2) | 2;
    }

    return -1;
}

std::string PathNode::toSVG() const {
    if (m_anchors.empty()) return "";
    std::string d = "M " + std::to_string(m_anchors[0].position.x) + " " + std::to_string(m_anchors[0].position.y);
    for (size_t i = 0; i < m_anchors.size() - 1; ++i) {
        const auto& p1 = m_anchors[i].handleOut;
        const auto& p2 = m_anchors[i+1].handleIn;
        const auto& p3 = m_anchors[i+1].position;
        d += " C " + std::to_string(p1.x) + " " + std::to_string(p1.y) + ", " +
             std::to_string(p2.x) + " " + std::to_string(p2.y) + ", " +
             std::to_string(p3.x) + " " + std::to_string(p3.y);
    }
    return "<path d=\"" + d + "\" />";
}

void calculateNormals(const PathNode& node) {
    (void)node;
}

} // namespace vectma
