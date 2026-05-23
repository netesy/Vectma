#pragma once

#include "core/CanvasNode.hpp"
#include "core/GRect.hpp"

namespace vectma {

class ArtboardNode : public CanvasNode {
public:
    ArtboardNode(const std::string& name = "Artboard", const GRect& bounds = {0, 0, 800, 600});

    std::string getClassName() const override { return "ArtboardNode"; }

    std::string getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    GRect getBounds() const { return m_bounds; }
    void setBounds(const GRect& bounds) { m_bounds = bounds; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;
    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

private:
    std::string m_name;
    GRect m_bounds;
};

} // namespace vectma
