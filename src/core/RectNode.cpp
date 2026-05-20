#include "core/RectNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

RectNode::RectNode(double x, double y, double w, double h)
    : m_x(x), m_y(y), m_w(w), m_h(h) {}

void RectNode::render(RenderPipeline& pipeline) const {
    pipeline.drawRect(*this);
}

bool RectNode::containsPoint(const GPoint& point) const {
    return point.x >= m_x && point.x <= m_x + m_w &&
           point.y >= m_y && point.y <= m_y + m_h;
}

GRect RectNode::computeBoundingBox() const {
    return GRect(m_x, m_y, m_w, m_h);
}

} // namespace vectma
