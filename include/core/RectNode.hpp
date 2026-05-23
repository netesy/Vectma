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
    std::unique_ptr<CanvasNode> clone() const override;

    double getX() const override { return m_x.value; }
    double getY() const override { return m_y.value; }
    double getWidth() const override { return m_w.value; }
    double getHeight() const override { return m_h.value; }

    double getW() const { return m_w.value; }
    double getH() const { return m_h.value; }

    void setX(double x);
    void setY(double y);
    void setW(double w);
    void setH(double h);

    void setPositionRemote(double x, double y, LamportTimestamp ts) override {
        m_x.update(x, ts);
        m_y.update(y, ts);
        markLayoutDirty();
        notifyDirty();
    }
    void setSizeRemote(double w, double h, LamportTimestamp ts) override {
        m_w.update(w, ts);
        m_h.update(h, ts);
        markLayoutDirty();
        notifyDirty();
    }

    void setXRemote(double x, LamportTimestamp ts) { m_x.update(x, ts); markLayoutDirty(); notifyDirty(); }
    void setYRemote(double y, LamportTimestamp ts) { m_y.update(y, ts); markLayoutDirty(); notifyDirty(); }
    void setWRemote(double w, LamportTimestamp ts) { m_w.update(w, ts); markLayoutDirty(); notifyDirty(); }
    void setHRemote(double h, LamportTimestamp ts) { m_h.update(h, ts); markLayoutDirty(); notifyDirty(); }

private:
    void notifyDirty();

    LWWProperty<double> m_x;
    LWWProperty<double> m_y;
    LWWProperty<double> m_w;
    LWWProperty<double> m_h;
};

} // namespace vectma
