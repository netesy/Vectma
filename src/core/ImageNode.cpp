#include "core/ImageNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

ImageNode::ImageNode(const std::vector<uint8_t>& data, double x, double y, double w, double h)
    : m_rawData(data) {
    LamportTimestamp ts{0, 0, 0};
    m_id.update({0, 0, 0}, ts);
    m_x.update(x, ts);
    m_y.update(y, ts);
    m_width.update(w, ts);
    m_height.update(h, ts);
}

void ImageNode::render(RenderPipeline& pipeline) const {
    pipeline.drawImage(*this);
}

bool ImageNode::containsPoint(const GPoint& point) const {
    return point.x >= getX() && point.x <= getX() + getWidth() &&
           point.y >= getY() && point.y <= getY() + getHeight();
}

GRect ImageNode::computeBoundingBox() const {
    return GRect(getX(), getY(), getWidth(), getHeight());
}

std::string ImageNode::toSVG() const {
    return "<image href=\"...\" />";
}

} // namespace vectma
