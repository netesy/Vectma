#pragma once
#include "core/CanvasNode.hpp"
#include <string>

namespace vectma {

class MasterComponentNode : public CanvasNode {
public:
    MasterComponentNode(const std::string& name = "Component");

    std::string getClassName() const override { return "MasterComponentNode"; }
    const std::string& getName() const { return m_name.value; }
    void setName(const std::string& name) { m_name.update(name, LamportClock::getInstance().tick()); }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    double getX() const override { return computeBoundingBox().x; }
    double getY() const override { return computeBoundingBox().y; }
    double getWidth() const override { return computeBoundingBox().width; }
    double getHeight() const override { return computeBoundingBox().height; }

private:
    LWWProperty<std::string> m_name;
};

} // namespace vectma
