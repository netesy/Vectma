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
    double getX() const { return m_x; }
    double getY() const { return m_y; }
    double getWidth() const { return m_width; }
    double getHeight() const { return m_height; }

private:
    std::vector<uint8_t> m_rawData;
    double m_x, m_y, m_width, m_height;
};

} // namespace vectma
