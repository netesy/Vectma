#include "core/EllipseNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>

namespace vectma {

EllipseNode::EllipseNode(double cx, double cy, double rx, double ry)
    : m_cx(cx), m_cy(cy), m_rx(rx), m_ry(ry) {}

void EllipseNode::render(RenderPipeline& pipeline) const {
    pipeline.drawEllipse(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool EllipseNode::containsPoint(const GPoint& point) const {
    double dx = (point.x - m_cx) / m_rx;
    double dy = (point.y - m_cy) / m_ry;
    return (dx * dx + dy * dy) <= 1.0;
}

GRect EllipseNode::computeBoundingBox() const {
    double halfStroke = getStrokeWidth() / 2.0;
    double x = m_cx - m_rx;
    double y = m_cy - m_ry;
    double w = m_rx * 2.0;
    double h = m_ry * 2.0;

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

std::string EllipseNode::toSVG() const {
    return "<ellipse cx=\"" + std::to_string(m_cx) + "\" cy=\"" + std::to_string(m_cy) +
           "\" rx=\"" + std::to_string(m_rx) + "\" ry=\"" + std::to_string(m_ry) + "\" />";
}

} // namespace vectma
