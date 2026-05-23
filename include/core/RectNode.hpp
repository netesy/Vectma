#pragma once

#include "core/CanvasNode.hpp"

namespace vectma {

class RectNode : public CanvasNode {
public:
    RectNode(double x, double y, double w, double h);

    std::string getClassName() const override { return "RectNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;

    double getX() const { return m_x; }
    double getY() const { return m_y; }
    double getW() const { return m_w; }
    double getH() const { return m_h; }

    void setX(double x) { m_x = x; }
    void setY(double y) { m_y = y; }
    void setW(double w) { m_w = w; }
    void setH(double h) { m_h = h; }

private:
    double m_x, m_y, m_w, m_h;
};

} // namespace vectma
