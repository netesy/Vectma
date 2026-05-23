#include "core/TextNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

TextNode::TextNode(const std::string& text, double x, double y) {
    LamportTimestamp ts{0, 0, 0};
    m_text.update(text, ts);
    m_x.update(x, ts);
    m_y.update(y, ts);
    m_fontSize.update(12.0f, ts);
}

void TextNode::render(RenderPipeline& pipeline) const {
    pipeline.drawText(*this);
}

bool TextNode::containsPoint(const GPoint& point) const {
    GRect bbox = computeBoundingBox();
    return point.x >= bbox.x && point.x <= bbox.x + bbox.width &&
           point.y >= bbox.y && point.y <= bbox.y + bbox.height;
}

GRect TextNode::computeBoundingBox() const {
    return GRect(getX(), getY() - getFontSize(), getText().length() * getFontSize() * 0.6, getFontSize());
}

std::unique_ptr<CanvasNode> TextNode::clone() const {
    auto copy = std::make_unique<TextNode>(getText(), getX(), getY());
    copy->setFontSize(getFontSize());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}

std::string TextNode::toSVG() const {
    return "<text x=\"" + std::to_string(getX()) + "\" y=\"" + std::to_string(getY()) +
           "\" font-size=\"" + std::to_string(getFontSize()) + "\">" + getText() + "</text>";
}

} // namespace vectma
