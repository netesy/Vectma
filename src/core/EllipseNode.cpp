#include "core/EllipseNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>

namespace vectma {

EllipseNode::EllipseNode(double cx, double cy, double rx, double ry) : CanvasNode() {
    LamportTimestamp ts{0, 0, 0};
    m_cx.update(cx, ts);
    m_cy.update(cy, ts);
    m_rx.update(rx, ts);
    m_ry.update(ry, ts);
}

void EllipseNode::render(RenderPipeline& pipeline) const {
    pipeline.drawEllipse(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool EllipseNode::containsPoint(const GPoint& point) const {
    double dx = (point.x - getCX()) / getRX();
    double dy = (point.y - getCY()) / getRY();
    return (dx * dx + dy * dy) <= 1.0;
}

GRect EllipseNode::computeBoundingBox() const {
    double sw = getStrokeWidth();
    return GRect(getCX() - getRX() - sw, getCY() - getRY() - sw, getRX() * 2 + sw * 2, getRY() * 2 + sw * 2);
}

std::unique_ptr<CanvasNode> EllipseNode::clone() const {
    auto copy = std::make_unique<EllipseNode>(getCX(), getCY(), getRX(), getRY());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}

std::string EllipseNode::toSVG() const {
    return "<ellipse cx=\"" + std::to_string(getCX()) + "\" cy=\"" + std::to_string(getCY()) +
           "\" rx=\"" + std::to_string(getRX()) + "\" ry=\"" + std::to_string(getRY()) + "\" />";
}

} // namespace vectma
