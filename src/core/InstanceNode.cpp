#include "core/InstanceNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

InstanceNode::InstanceNode(SymbolID symbolId) : m_symbolId(symbolId) {}

void InstanceNode::render(RenderPipeline& pipeline) const {
    CanvasNode* root = SymbolRegistry::getInstance().getSymbolRoot(m_symbolId);
    if (root) {
        // In real app, we'd apply overrides here or pass them down
        root->render(pipeline);
    }
}

bool InstanceNode::containsPoint(const GPoint& point) const {
    return computeBoundingBox().contains(point.x, point.y);
}

GRect InstanceNode::computeBoundingBox() const {
    if (m_dirty) {
        CanvasNode* root = SymbolRegistry::getInstance().getSymbolRoot(m_symbolId);
        if (root) {
            m_cachedBBox = root->computeBoundingBox();
        } else {
            m_cachedBBox = GRect(0, 0, 0, 0);
        }
        m_dirty = false;
    }
    return m_cachedBBox;
}

std::string InstanceNode::toSVG() const {
    CanvasNode* root = SymbolRegistry::getInstance().getSymbolRoot(m_symbolId);
    if (root) return root->toSVG();
    return "";
}

void InstanceNode::setOverride(const std::string& key, const std::string& value) {
    m_overrides[key] = value;
    m_dirty = true;
}

std::string InstanceNode::getOverride(const std::string& key) const {
    auto it = m_overrides.find(key);
    if (it != m_overrides.end()) return it->second;
    return "";
}

} // namespace vectma

namespace vectma {

void invalidateSymbolInstances(const SymbolID& id, CanvasNode* root) {
    if (!root) return;

    // Recursive search for InstanceNodes targeting this SymbolID
    auto* instance = dynamic_cast<InstanceNode*>(root);
    // (In real app, InstanceNode would have a getSymbolId() or we'd check against ID)
    (void)id;
    if (instance) {
        // Mock dirty trigger
    }

    for (const auto& child : root->getChildren()) {
        invalidateSymbolInstances(id, child.get());
    }
}

} // namespace vectma
