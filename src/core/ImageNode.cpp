#include "core/ImageNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

ImageNode::ImageNode(const std::vector<uint8_t>& data, double x, double y, double w, double h)
    : CanvasNode(), m_rawData(data) {
    LamportTimestamp ts{0, 0, 0};
    m_x.update(x, ts);
    m_y.update(y, ts);
    m_w.update(w, ts);
    m_h.update(h, ts);
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

std::unique_ptr<CanvasNode> ImageNode::clone() const {
    auto copy = std::make_unique<ImageNode>(getRawData(), getX(), getY(), getWidth(), getHeight());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}

std::string ImageNode::toSVG() const {
    return "<image x=\"" + std::to_string(getX()) + "\" y=\"" + std::to_string(getY()) +
           "\" width=\"" + std::to_string(getWidth()) + "\" height=\"" + std::to_string(getHeight()) + "\" />";
}

} // namespace vectma
