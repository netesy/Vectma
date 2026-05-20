#include "core/RectNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

RectNode::RectNode(double x, double y, double w, double h)
    : m_x(x), m_y(y), m_w(w), m_h(h) {}

void RectNode::render(RenderPipeline& pipeline) const {
    pipeline.drawRect(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool RectNode::containsPoint(const GPoint& point) const {
    return point.x >= m_x && point.x <= m_x + m_w &&
           point.y >= m_y && point.y <= m_y + m_h;
}

GRect RectNode::computeBoundingBox() const {
    double halfStroke = getStrokeWidth() / 2.0;
    double x = m_x;
    double y = m_y;
    double w = m_w;
    double h = m_h;

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
    // Inside: no inflation needed beyond the primitive border

    return GRect(x, y, w, h);
}

} // namespace vectma
