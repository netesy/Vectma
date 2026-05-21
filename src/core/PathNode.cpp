#include "core/PathNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>
#include <algorithm>

namespace vectma {

PathNode::PathNode() = default;

PathNode::PathNode(const std::vector<BezierAnchor>& anchors)
    : m_anchors(anchors) {}

void PathNode::setAnchors(const std::vector<BezierAnchor>& anchors) {
    m_anchors = anchors;
    m_geometry_dirty = true;
}

void PathNode::addAnchor(const BezierAnchor& anchor) {
    m_anchors.push_back(anchor);
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

    // Start with base geometry
    auto current_data = std::make_unique<PathData>(m_anchors, m_isClosed);

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
    // In a real implementation, we might pass the compiled path to the pipeline
    // Dispatch active node to the render pipeline
    pipeline.drawPath(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool PathNode::containsPoint(const GPoint& point) const {
    return hitTestAnchors(point, 5.0f) != -1;
}

GRect PathNode::computeBoundingBox() const {
    const auto& data = getCompiledPath();
    if (data.anchors.empty()) return GRect(0, 0, 0, 0);

    double minX = data.anchors[0].position.x;
    double minY = data.anchors[0].position.y;
    double maxX = minX;
    double maxY = minY;

    auto update = [&](const Point2D& p) {
        minX = std::min(minX, p.x);
        minY = std::min(minY, p.y);
        maxX = std::max(maxX, p.x);
        maxY = std::max(maxY, p.y);
    };

    for (const auto& anchor : data.anchors) {
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
    const auto& data = getCompiledPath();
    if (data.anchors.empty()) return "";
    std::string d = "M " + std::to_string(data.anchors[0].position.x) + " " + std::to_string(data.anchors[0].position.y);
    for (size_t i = 0; i < data.anchors.size() - 1; ++i) {
        const auto& p1 = data.anchors[i].handleOut;
        const auto& p2 = data.anchors[i+1].handleIn;
        const auto& p3 = data.anchors[i+1].position;
        d += " C " + std::to_string(p1.x) + " " + std::to_string(p1.y) + ", " +
             std::to_string(p2.x) + " " + std::to_string(p2.y) + ", " +
             std::to_string(p3.x) + " " + std::to_string(p3.y);
    }
    return "<path d=\"" + d + "\" />";
}

} // namespace vectma
