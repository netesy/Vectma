#include "core/CompoundShapeNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

CompoundShapeNode::CompoundShapeNode(BooleanOpType op) {
    m_opType.update(op, LamportClock::getInstance().tick());
}

void CompoundShapeNode::setOpType(BooleanOpType op) {
    if (m_opType.update(op, LamportClock::getInstance().tick())) {
        markDirty();
    }
}

void CompoundShapeNode::markDirty() {
    m_pathDirty = true;
    if (m_parent) {
        if (auto* parentCompound = dynamic_cast<CompoundShapeNode*>(m_parent)) {
            parentCompound->markDirty();
        }
    }
}

void CompoundShapeNode::render(RenderPipeline& pipeline) const {
    pipeline.drawCompoundShape(*this);
}

bool CompoundShapeNode::containsPoint(const GPoint& point) const {
    // For hit testing in a production engine, we'd use the resolved path cache.
    // As a fallback, we check children if Union, or first child if Subtract.
    for (const auto& child : m_children) {
        if (child->containsPoint(point)) return true;
    }
    return false;
}

GRect CompoundShapeNode::computeBoundingBox() const {
    GRect bbox;
    for (const auto& child : m_children) {
        bbox = bbox.united(child->computeBoundingBox());
    }
    return bbox;
}

std::string CompoundShapeNode::toSVG() const {
    std::string svg = "<g><!-- Compound Shape -->";
    for (const auto& child : m_children) svg += child->toSVG();
    svg += "</g>";
    return svg;
}

} // namespace vectma
