#include "core/ArtboardNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

ArtboardNode::ArtboardNode(const std::string& name, const GRect& bounds) : CanvasNode() {
    LamportTimestamp ts{0, 0, 0};
    m_name.update(name, ts);
    m_bounds.update(bounds, ts);
}

void ArtboardNode::render(RenderPipeline& pipeline) const {
    if (!isVisible()) return;
    pipeline.pushClipRect(getBounds());
    for (const auto& child : m_children) {
        if (child->isVisible()) child->render(pipeline);
    }
    pipeline.popClipRect();
}

bool ArtboardNode::containsPoint(const GPoint& point) const {
    if (!isVisible() || isLocked()) return false;
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->containsPoint(point)) return true;
    }
    return getBounds().contains(point.x, point.y);
}

GRect ArtboardNode::computeBoundingBox() const {
    return getBounds();
}

std::unique_ptr<CanvasNode> ArtboardNode::clone() const {
    auto copy = std::make_unique<ArtboardNode>(getName(), getBounds());
    for(const auto& child : m_children) copy->addChild(child->clone());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}

std::string ArtboardNode::toSVG() const {
    GRect b = getBounds();
    std::string svg = "<svg id=\"" + getName() + "\" x=\"" + std::to_string(b.x) +
                      "\" y=\"" + std::to_string(b.y) +
                      "\" width=\"" + std::to_string(b.width) +
                      "\" height=\"" + std::to_string(b.height) + "\">";
    for (const auto& child : m_children) svg += child->toSVG();
    svg += "</svg>";
    return svg;
}

} // namespace vectma
