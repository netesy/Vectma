#include "core/LayerNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

LayerNode::LayerNode(const std::string& name) : m_name(name) {}

void LayerNode::render(RenderPipeline& pipeline) const {
    if (!isVisible()) return;

    // Propagation of opacity is handled by the renderer usually,
    // or by multiplying it down the tree.
    for (const auto& child : m_children) {
        child->render(pipeline);
    }
}

bool LayerNode::containsPoint(const GPoint& point) const {
    if (!isVisible() || isLocked()) return false;

    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->containsPoint(point)) return true;
    }
    return false;
}

GRect LayerNode::computeBoundingBox() const {
    if (m_children.empty()) return GRect(0, 0, 0, 0);

    GRect bbox = m_children[0]->computeBoundingBox();
    for (size_t i = 1; i < m_children.size(); ++i) {
        bbox = bbox.united(m_children[i]->computeBoundingBox());
    }
    return bbox;
}

std::string LayerNode::toSVG() const {
    std::string svg = "<g id=\"" + m_name + "\" opacity=\"" + std::to_string(m_opacity) + "\">";
    for (const auto& child : m_children) {
        svg += child->toSVG();
    }
    svg += "</g>";
    return svg;
}

} // namespace vectma
