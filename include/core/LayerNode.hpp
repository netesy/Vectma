#pragma once

#include "core/CanvasNode.hpp"

namespace vectma {

class LayerNode : public CanvasNode {
public:
    LayerNode(const std::string& name = "Layer");

    std::string getClassName() const override { return "LayerNode"; }
    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;
    std::string toSVG() const override;

private:
    std::string m_name;
};

} // namespace vectma
