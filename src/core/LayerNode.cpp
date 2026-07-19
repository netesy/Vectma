#include "core/LayerNode.hpp"
#include <algorithm>

namespace vectma {

LayerNode::LayerNode(const std::string& name) : CanvasNode() {
    m_nodeType = NodeType::Layer;
    m_name.update(name, {0, 0, 0});
}

void LayerNode::render(RenderPipeline& pipeline) const {
    for (const auto& child : m_children) {
        if (child->isVisible()) child->render(pipeline);
    }
}

bool LayerNode::containsPoint(const GPoint& point) const {
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->containsPoint(point)) return true;
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

std::unique_ptr<CanvasNode> LayerNode::clone() const {
    auto copy = std::make_unique<LayerNode>(getName());
    for(const auto& child : m_children) copy->addChild(child->clone());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}

std::string LayerNode::toSVG() const {
    std::string svg = "<g id=\"" + getName() + "\" opacity=\"" + std::to_string(getOpacity()) + "\">";
    for (const auto& child : m_children) svg += child->toSVG();
    svg += "</g>";
    return svg;
}

} // namespace vectma
