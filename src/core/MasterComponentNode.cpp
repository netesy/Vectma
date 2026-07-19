#include "core/MasterComponentNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

MasterComponentNode::MasterComponentNode(const std::string& name) : CanvasNode() {
    m_nodeType = NodeType::MasterComponent;
    m_name.update(name, {0, 0, 0});
}

void MasterComponentNode::render(RenderPipeline& pipeline) const {
    if (!isVisible()) return;
    for (const auto& child : m_children) {
        if (child->isVisible()) {
            child->render(pipeline);
        }
    }
}

bool MasterComponentNode::containsPoint(const GPoint& point) const {
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->containsPoint(point)) return true;
    }
    return false;
}

GRect MasterComponentNode::computeBoundingBox() const {
    GRect bbox;
    for (const auto& child : m_children) {
        bbox = bbox.united(child->computeBoundingBox());
    }
    return bbox;
}

std::unique_ptr<CanvasNode> MasterComponentNode::clone() const {
    auto copy = std::make_unique<MasterComponentNode>(getName());
    for(const auto& child : m_children) copy->addChild(child->clone());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}
std::string MasterComponentNode::toSVG() const {
    std::string svg = "<g id=\"" + getName() + "\">";
    for (const auto& child : m_children) svg += child->toSVG();
    svg += "</g>";
    return svg;
}

} // namespace vectma
