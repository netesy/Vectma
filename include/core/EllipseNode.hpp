#pragma once

#include "core/CanvasNode.hpp"

namespace vectma {

class EllipseNode : public CanvasNode {
public:
    EllipseNode(double cx, double cy, double rx, double ry);

    std::string getClassName() const override { return "EllipseNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    double getCX() const { return m_cx; }
    double getCY() const { return m_cy; }
    double getRX() const { return m_rx; }
    double getRY() const { return m_ry; }

private:
    double m_cx, m_cy, m_rx, m_ry;
};

} // namespace vectma
