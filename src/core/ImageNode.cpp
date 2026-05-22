#include "core/ImageNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

ImageNode::ImageNode(const std::vector<uint8_t>& data, double x, double y, double w, double h)
    : m_rawData(data), m_x(x), m_y(y), m_width(w), m_height(h) {}

void ImageNode::render(RenderPipeline& pipeline) const {
    // We need to add drawImage to RenderPipeline if we want proper visitor dispatch
    pipeline.drawImage(*this);
}

bool ImageNode::containsPoint(const GPoint& point) const {
    return point.x >= m_x && point.x <= m_x + m_width &&
           point.y >= m_y && point.y <= m_y + m_height;
}

GRect ImageNode::computeBoundingBox() const {
    return GRect(m_x, m_y, m_width, m_height);
}

std::string ImageNode::toSVG() const {
    return "<image href=\"...\" />";
}

} // namespace vectma
