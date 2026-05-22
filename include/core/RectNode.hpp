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

    double getX() const { return m_x.value; }
    double getY() const { return m_y.value; }
    double getW() const { return m_w.value; }
    double getH() const { return m_h.value; }

    void setX(double x) { m_x.update(x, LamportClock::getInstance().tick()); }
    void setY(double y) { m_y.update(y, LamportClock::getInstance().tick()); }
    void setW(double w) { m_w.update(w, LamportClock::getInstance().tick()); }
    void setH(double h) { m_h.update(h, LamportClock::getInstance().tick()); }

    void setXRemote(double x, LamportTimestamp ts) { m_x.update(x, ts); }
    void setYRemote(double y, LamportTimestamp ts) { m_y.update(y, ts); }
    void setWRemote(double w, LamportTimestamp ts) { m_w.update(w, ts); }
    void setHRemote(double h, LamportTimestamp ts) { m_h.update(h, ts); }

private:
    LWWProperty<double> m_x;
    LWWProperty<double> m_y;
    LWWProperty<double> m_w;
    LWWProperty<double> m_h;
};

} // namespace vectma
