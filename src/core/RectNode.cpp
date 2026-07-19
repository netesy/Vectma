#include "core/RectNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include "core/CompoundShapeNode.hpp"

namespace vectma {

RectNode::RectNode(double x, double y, double w, double h) : CanvasNode() {
    m_nodeType = NodeType::Rect;
    LamportTimestamp ts{0, 0, 0};
    m_x.update(x, ts);
    m_y.update(y, ts);
    m_w.update(w, ts);
    m_h.update(h, ts);
}

void RectNode::render(RenderPipeline& pipeline) const {
    pipeline.drawRect(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool RectNode::containsPoint(const GPoint& point) const {
    return point.x >= getX() && point.x <= getX() + getWidth() &&
           point.y >= getY() && point.y <= getY() + getHeight();
}

GRect RectNode::computeBoundingBox() const {
    double sw = getStrokeWidth();
    return GRect(getX() - sw, getY() - sw, getWidth() + sw*2, getHeight() + sw*2);
}

std::unique_ptr<CanvasNode> RectNode::clone() const {
    auto copy = std::make_unique<RectNode>(getX(), getY(), getWidth(), getHeight());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}

std::string RectNode::toSVG() const {
    return "<rect x=\"" + std::to_string(getX()) + "\" y=\"" + std::to_string(getY()) +
           "\" width=\"" + std::to_string(getWidth()) + "\" height=\"" + std::to_string(getHeight()) + "\" />";
}

void RectNode::setX(double x) { m_x.update(x, LamportClock::getInstance().tick()); markLayoutDirty(); notifyDirty(); }
void RectNode::setY(double y) { m_y.update(y, LamportClock::getInstance().tick()); markLayoutDirty(); notifyDirty(); }
void RectNode::setW(double w) { m_w.update(w, LamportClock::getInstance().tick()); markLayoutDirty(); notifyDirty(); }
void RectNode::setH(double h) { m_h.update(h, LamportClock::getInstance().tick()); markLayoutDirty(); notifyDirty(); }

void RectNode::notifyDirty() {
    if (m_parent) {
        if (auto* cs = dynamic_cast<CompoundShapeNode*>(m_parent)) {
            cs->markDirty();
        }
    }
}

} // namespace vectma
