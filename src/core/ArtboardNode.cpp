#include "core/ArtboardNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

ArtboardNode::ArtboardNode(const std::string& name, const GRect& bounds) {
    LamportTimestamp ts{0, 0, 0};
    m_name.update(name, ts);
    m_bounds.update(bounds, ts);
}

void ArtboardNode::render(RenderPipeline& pipeline) const {
    if (!isVisible()) return;
    pipeline.pushClipRect(getBounds());
    for (const auto& child : m_children) {
        child->render(pipeline);
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

std::string ArtboardNode::toSVG() const {
    std::string svg = "<svg id=\"" + getName() + "\" x=\"" + std::to_string(getBounds().x) +
                      "\" y=\"" + std::to_string(getBounds().y) +
                      "\" width=\"" + std::to_string(getBounds().width) +
                      "\" height=\"" + std::to_string(getBounds().height) + "\">";
    for (const auto& child : m_children) {
        svg += child->toSVG();
    }
    svg += "</svg>";
    return svg;
}

} // namespace vectma
