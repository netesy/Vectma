#include "core/ArtboardNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

ArtboardNode::ArtboardNode(const std::string& name, const GRect& bounds)
    : m_name(name), m_bounds(bounds) {}

void ArtboardNode::render(RenderPipeline& pipeline) const {
    if (!isVisible()) return;

    pipeline.pushClipRect(m_bounds);

    // Draw background/canvas for artboard if needed
    // ...

    for (const auto& child : m_children) {
        child->render(pipeline);
    }

    pipeline.popClipRect();
}

bool ArtboardNode::containsPoint(const GPoint& point) const {
    // Artboard itself can be hit tested by its bounds
    if (!isVisible() || isLocked()) return false;

    // First check children
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->containsPoint(point)) return true;
    }

    return m_bounds.contains(point.x, point.y);
}

GRect ArtboardNode::computeBoundingBox() const {
    return m_bounds;
}

std::string ArtboardNode::toSVG() const {
    std::string svg = "<svg id=\"" + m_name + "\" x=\"" + std::to_string(m_bounds.x) +
                      "\" y=\"" + std::to_string(m_bounds.y) +
                      "\" width=\"" + std::to_string(m_bounds.width) +
                      "\" height=\"" + std::to_string(m_bounds.height) + "\">";
    for (const auto& child : m_children) {
        svg += child->toSVG();
    }
    svg += "</svg>";
    return svg;
}

} // namespace vectma
