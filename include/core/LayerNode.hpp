#pragma once
#include "core/CanvasNode.hpp"

namespace vectma {

class LayerNode : public CanvasNode {
public:
    LayerNode(const std::string& name = "Layer");

    std::string getClassName() const override { return "LayerNode"; }
    const std::string& getName() const { return m_name.value; }
    void setName(const std::string& name) { m_name.update(name, LamportClock::getInstance().tick()); }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;
    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

private:
    LWWProperty<std::string> m_name;
};

} // namespace vectma
