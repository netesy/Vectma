#pragma once
#include "core/CanvasNode.hpp"
#include "core/PropertyOverride.hpp"
#include <string>

namespace vectma {

class MasterComponentNode;

class ComponentInstanceNode : public CanvasNode {
public:
    ComponentInstanceNode(MasterComponentNode* master);

    std::string getClassName() const override { return "ComponentInstanceNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    MasterComponentNode* getMaster() const { return m_master; }

    void setOverride(const std::string& path, PropertyVariant value);
    const OverrideMap& getOverrides() const { return m_overrides; }

    double getX() const override { return computeBoundingBox().x; }
    double getY() const override { return computeBoundingBox().y; }
    double getWidth() const override { return computeBoundingBox().width; }
    double getHeight() const override { return computeBoundingBox().height; }

private:
    MasterComponentNode* m_master;
    OverrideMap m_overrides;
};

} // namespace vectma
