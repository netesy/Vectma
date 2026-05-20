#include "core/EllipseNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>

namespace vectma {

EllipseNode::EllipseNode(double cx, double cy, double rx, double ry)
    : m_cx(cx), m_cy(cy), m_rx(rx), m_ry(ry) {}

void EllipseNode::render(RenderPipeline& pipeline) const {
    pipeline.drawEllipse(*this);
}

bool EllipseNode::containsPoint(const GPoint& point) const {
    if (m_rx <= 0 || m_ry <= 0) return false;
    double dx = (point.x - m_cx) / m_rx;
    double dy = (point.y - m_cy) / m_ry;
    return (dx * dx + dy * dy) <= 1.0;
}

GRect EllipseNode::computeBoundingBox() const {
    return GRect(m_cx - m_rx, m_cy - m_ry, m_rx * 2, m_ry * 2);
}

} // namespace vectma
