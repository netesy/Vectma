#pragma once

#include "core/CanvasNode.hpp"
#include <string>

namespace vectma {

class MasterComponentNode : public CanvasNode {
public:
    MasterComponentNode(const std::string& name = "Component");

    std::string getClassName() const override { return "MasterComponentNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

private:
    std::string m_name;
};

} // namespace vectma
