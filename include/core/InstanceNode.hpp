#pragma once

#include "core/CanvasNode.hpp"
#include "core/SymbolRegistry.hpp"
#include <map>

namespace vectma {

class InstanceNode : public CanvasNode {
public:
    InstanceNode(SymbolID symbolId);

    std::string getClassName() const override { return "InstanceNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;
    std::string toSVG() const override;

    void setOverride(const std::string& key, const std::string& value);
    std::string getOverride(const std::string& key) const;

private:
    SymbolID m_symbolId;
    std::map<std::string, std::string> m_overrides;
    mutable bool m_dirty = true;
    mutable GRect m_cachedBBox;
};

} // namespace vectma

namespace vectma {
    // Helper to invalidate all instances of a symbol
    void invalidateSymbolInstances(const SymbolID& id, CanvasNode* root);
}
