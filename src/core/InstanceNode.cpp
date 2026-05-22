#include "core/InstanceNode.hpp"
#include "core/SymbolRegistry.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

InstanceNode::InstanceNode(const std::string& symbolId) {
    LamportTimestamp ts{0, 0, 0};
    m_symbolId.update(symbolId, ts);
    m_transform.update(GTransform::Identity(), ts);
}

void InstanceNode::render(RenderPipeline& pipeline) const {
    auto symbol = SymbolRegistry::getInstance().getSymbolRoot(getSymbolId());
    if (symbol) {
        symbol->render(pipeline);
    }
}

bool InstanceNode::containsPoint(const GPoint& point) const {
    auto symbol = SymbolRegistry::getInstance().getSymbolRoot(getSymbolId());
    if (!symbol) return false;
    GPoint localPoint = getTransform().inverse().map(point);
    return symbol->containsPoint(localPoint);
}

GRect InstanceNode::computeBoundingBox() const {
    auto symbol = SymbolRegistry::getInstance().getSymbolRoot(getSymbolId());
    if (!symbol) return GRect();
    return symbol->computeBoundingBox();
}

std::string InstanceNode::toSVG() const {
    return "<use href=\"#symbol-" + getSymbolId() + "\" />";
}

} // namespace vectma
