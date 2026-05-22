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

    const std::vector<uint8_t>& getRawData() const { return m_rawData; }
    double getX() const { return m_x.value; }
    double getY() const { return m_y.value; }
    double getWidth() const { return m_width.value; }
    double getHeight() const { return m_height.value; }

    void setX(double x) { m_x.update(x, LamportClock::getInstance().tick()); }
    void setY(double y) { m_y.update(y, LamportClock::getInstance().tick()); }
    void setWidth(double w) { m_width.update(w, LamportClock::getInstance().tick()); }
    void setHeight(double h) { m_height.update(h, LamportClock::getInstance().tick()); }

private:
    std::vector<uint8_t> m_rawData;
    LWWProperty<double> m_x;
    LWWProperty<double> m_y;
    LWWProperty<double> m_width;
    LWWProperty<double> m_height;
};

} // namespace vectma
