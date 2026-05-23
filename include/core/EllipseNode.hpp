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

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    double getX() const override { return m_cx.value - m_rx.value; }
    double getY() const override { return m_cy.value - m_ry.value; }
    double getWidth() const override { return m_rx.value * 2.0; }
    double getHeight() const override { return m_ry.value * 2.0; }

    double getCX() const { return m_cx.value; }
    double getCY() const { return m_cy.value; }
    double getRX() const { return m_rx.value; }
    double getRY() const { return m_ry.value; }

    void setCX(double cx) { m_cx.update(cx, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setCY(double cy) { m_cy.update(cy, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setRX(double rx) { m_rx.update(rx, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setRY(double ry) { m_ry.update(ry, LamportClock::getInstance().tick()); markLayoutDirty(); }

    void setPositionRemote(double x, double y, LamportTimestamp ts) override {
        double dx = x - getX();
        double dy = y - getY();
        m_cx.update(m_cx.value + dx, ts);
        m_cy.update(m_cy.value + dy, ts);
        markLayoutDirty();
    }

    void setSizeRemote(double w, double h, LamportTimestamp ts) override {
        m_rx.update(w / 2.0, ts);
        m_ry.update(h / 2.0, ts);
        markLayoutDirty();
    }

private:
    LWWProperty<double> m_cx;
    LWWProperty<double> m_cy;
    LWWProperty<double> m_rx;
    LWWProperty<double> m_ry;
};

} // namespace vectma
