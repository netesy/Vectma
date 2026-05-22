#include "core/LayerNode.hpp"
#include <algorithm>

namespace vectma {

LayerNode::LayerNode(const std::string& name) : m_name(name) {}

void LayerNode::render(RenderPipeline& pipeline) const {
    for (const auto& child : m_children) {
        if (child->isVisible()) {
            child->render(pipeline);
        }
    }
}

bool LayerNode::containsPoint(const GPoint& point) const {
    for (const auto& child : m_children) {
        if (child->containsPoint(point)) return true;
    }
    return false;
}

GRect LayerNode::computeBoundingBox() const {
    GRect bbox;
    for (const auto& child : m_children) {
        bbox = bbox.united(child->computeBoundingBox());
    }
    return bbox;
}

std::string LayerNode::toSVG() const {
    std::string svg = "<g id=\"" + m_name + "\" opacity=\"" + std::to_string(getOpacity()) + "\">";
    for (const auto& child : m_children) {
        svg += child->toSVG();
    }
    svg += "</g>";
    return svg;
}

} // namespace vectma
