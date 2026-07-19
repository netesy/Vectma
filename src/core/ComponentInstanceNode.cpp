#include "core/ComponentInstanceNode.hpp"
#include "core/MasterComponentNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

ComponentInstanceNode::ComponentInstanceNode(MasterComponentNode* master) : m_master(master) {
    m_nodeType = NodeType::ComponentInstance;
}

void ComponentInstanceNode::render(RenderPipeline& pipeline) const {
    if (!m_master || !isVisible()) return;

    pipeline.pushOverrideContext(&m_overrides);
    m_master->render(pipeline);
    pipeline.popOverrideContext();
}

bool ComponentInstanceNode::containsPoint(const GPoint& point) const {
    if (!m_master) return false;
    // Simplified: check against master's geometry in instance space
    return m_master->containsPoint(point);
}

GRect ComponentInstanceNode::computeBoundingBox() const {
    if (!m_master) return GRect();
    return m_master->computeBoundingBox();
}

std::unique_ptr<CanvasNode> ComponentInstanceNode::clone() const {
    auto copy = std::make_unique<ComponentInstanceNode>(m_master);
    for(const auto& [k, v] : m_overrides) copy->setOverride(k, v);
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}
std::string ComponentInstanceNode::toSVG() const {
    if (!m_master) return "";
    return m_master->toSVG();
}

void ComponentInstanceNode::setOverride(const std::string& path, PropertyVariant value) {
    m_overrides[path] = value;
}

} // namespace vectma
