#pragma once

#include "core/CanvasNode.hpp"
#include "core/GTransform.hpp"
#include <map>

namespace vectma {

class InstanceNode : public CanvasNode {
public:
    InstanceNode(const std::string& symbolId);

    std::string getClassName() const override { return "InstanceNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;
    std::string toSVG() const override;

    std::string getSymbolId() const { return m_symbolId.value; }
    GTransform getTransform() const { return m_transform.value; }
    void setTransform(const GTransform& t) { m_transform.update(t, LamportClock::getInstance().tick()); }

private:
    LWWProperty<std::string> m_symbolId;
    LWWProperty<GTransform> m_transform;
};

} // namespace vectma
