#pragma once
#include "core/CanvasNode.hpp"
#include <vector>
#include <cstdint>

namespace vectma {

class ImageNode : public CanvasNode {
public:
    ImageNode(const std::vector<uint8_t>& data, double x, double y, double w, double h);

    std::string getClassName() const override { return "ImageNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    double getX() const override { return m_x.value; }
    double getY() const override { return m_y.value; }
    double getWidth() const override { return m_w.value; }
    double getHeight() const override { return m_h.value; }

    void setX(double x) { m_x.update(x, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setY(double y) { m_y.update(y, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setW(double w) { m_w.update(w, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setH(double h) { m_h.update(h, LamportClock::getInstance().tick()); markLayoutDirty(); }

    void setPositionRemote(double x, double y, LamportTimestamp ts) override {
        m_x.update(x, ts);
        m_y.update(y, ts);
        markLayoutDirty();
    }
    void setSizeRemote(double w, double h, LamportTimestamp ts) override {
        m_w.update(w, ts);
        m_h.update(h, ts);
        markLayoutDirty();
    }

    const std::vector<uint8_t>& getRawData() const { return m_rawData; }

private:
    std::vector<uint8_t> m_rawData;
    LWWProperty<double> m_x;
    LWWProperty<double> m_y;
    LWWProperty<double> m_w;
    LWWProperty<double> m_h;
};

} // namespace vectma
