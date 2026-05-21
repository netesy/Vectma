#include "core/PathNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>
#include <algorithm>

namespace vectma {

PathNode::PathNode() = default;

PathNode::PathNode(const std::vector<BezierAnchor>& anchors)
    : m_anchors(anchors) {
    m_baseContours.emplace_back(anchors, m_isClosed);
}

PathNode::PathNode(const std::vector<Contour>& contours)
    : m_baseContours(contours) {
    if (!m_baseContours.empty()) {
        m_anchors = m_baseContours[0].anchors;
        m_isClosed = m_baseContours[0].isClosed;
    }
}

void PathNode::setAnchors(const std::vector<BezierAnchor>& anchors) {
    m_anchors = anchors;
    m_baseContours.clear();
    m_baseContours.emplace_back(anchors, m_isClosed);
    m_geometry_dirty = true;
}

void PathNode::setBaseContours(const std::vector<Contour>& contours) {
    m_baseContours = contours;
    if (!m_baseContours.empty()) {
        m_anchors = m_baseContours[0].anchors;
        m_isClosed = m_baseContours[0].isClosed;
    }
    m_geometry_dirty = true;
}

void PathNode::setClosed(bool closed) {
    if (m_isClosed != closed) {
        m_isClosed = closed;
        if (!m_baseContours.empty()) m_baseContours[0].isClosed = closed;
        m_geometry_dirty = true;
    }
}

void PathNode::addAnchor(const BezierAnchor& anchor) {
    m_anchors.push_back(anchor);
    if (!m_baseContours.empty()) {
        m_baseContours[0].anchors.push_back(anchor);
    } else {
        m_baseContours.emplace_back(m_anchors, m_isClosed);
    }
    m_geometry_dirty = true;
}

void PathNode::addModifier(std::unique_ptr<Modifier> modifier) {
    m_modifier_stack.push_back(std::move(modifier));
}

void PathNode::updatePipelineCache() const {
    bool stack_dirty = false;
    for (const auto& mod : m_modifier_stack) {
        if (mod->isDirty()) {
            stack_dirty = true;
            break;
        }
    }

    if (!m_geometry_dirty && !stack_dirty && m_cached_compiled_path) {
        return;
    }

    auto current_data = std::make_unique<PathData>();
    current_data->contours = m_baseContours;

    // Apply modifier stack
    for (const auto& mod : m_modifier_stack) {
        current_data = mod->apply(*current_data);
    }

    m_cached_compiled_path = std::move(current_data);
    m_geometry_dirty = false;
}

const PathData& PathNode::getCompiledPath() const {
    updatePipelineCache();
    return *m_cached_compiled_path;
}

void PathNode::render(RenderPipeline& pipeline) const {
    updatePipelineCache();
    pipeline.drawPath(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool PathNode::containsPoint(const GPoint& point) const {
    return hitTestAnchors(point, 5.0f) != -1;
}

GRect PathNode::computeBoundingBox() const {
    const auto& data = getCompiledPath();
    if (data.contours.empty()) return GRect(0, 0, 0, 0);

    bool first = true;
    double minX = 0, minY = 0, maxX = 0, maxY = 0;

    auto update = [&](const Point2D& p) {
        if (first) {
            minX = maxX = p.x;
            minY = maxY = p.y;
            first = false;
        } else {
            minX = std::min(minX, p.x);
            minY = std::min(minY, p.y);
            maxX = std::max(maxX, p.x);
            maxY = std::max(maxY, p.y);
        }
    };

    for (const auto& contour : data.contours) {
        for (const auto& anchor : contour.anchors) {
            update(anchor.position);
            update(anchor.handleIn);
            update(anchor.handleOut);
        }
    }

    if (first) return GRect(0, 0, 0, 0);

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
    const auto& data = getCompiledPath();
    if (data.contours.empty()) return "";

    std::string full_d = "";
    for (const auto& contour : data.contours) {
        if (contour.anchors.empty()) continue;

        std::string d = "M " + std::to_string(contour.anchors[0].position.x) + " " + std::to_string(contour.anchors[0].position.y);
        for (size_t i = 0; i < contour.anchors.size() - 1; ++i) {
            const auto& p1 = contour.anchors[i].handleOut;
            const auto& p2 = contour.anchors[i+1].handleIn;
            const auto& p3 = contour.anchors[i+1].position;
            d += " C " + std::to_string(p1.x) + " " + std::to_string(p1.y) + ", " +
                 std::to_string(p2.x) + " " + std::to_string(p2.y) + ", " +
                 std::to_string(p3.x) + " " + std::to_string(p3.y);
        }
        if (contour.isClosed && contour.anchors.size() > 1) {
            const auto& p1 = contour.anchors.back().handleOut;
            const auto& p2 = contour.anchors.front().handleIn;
            const auto& p3 = contour.anchors.front().position;
             d += " C " + std::to_string(p1.x) + " " + std::to_string(p1.y) + ", " +
                 std::to_string(p2.x) + " " + std::to_string(p2.y) + ", " +
                 std::to_string(p3.x) + " " + std::to_string(p3.y);
            d += " Z";
        }
        full_d += d + " ";
    }

    return "<path d=\"" + full_d + "\" />";
}

} // namespace vectma
