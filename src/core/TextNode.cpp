#include "core/TextNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>

namespace vectma {

TextNode::TextNode(const std::string& text, float x, float y, float size)
    : text_buffer(text), font_size(size), m_position(x, y) {}

void TextNode::render(RenderPipeline& pipeline) const {
    pipeline.drawText(*this);
}

bool TextNode::containsPoint(const GPoint& point) const {
    return computeBoundingBox().contains(point.x, point.y);
}

GRect TextNode::computeBoundingBox() const {
    float width = text_buffer.length() * font_size * 0.6f;
    float height = font_size * leading;
    return GRect(m_position.x, m_position.y, width, height);
}

std::unique_ptr<PathNode> TextNode::toPathNode() const {
    std::vector<BezierAnchor> anchors;
    float current_x = m_position.x;
    for (char c : text_buffer) {
        if (c == ' ') {
            current_x += font_size * 0.3f;
            continue;
        }

        float char_w = font_size * 0.5f;
        float char_h = font_size;

        anchors.emplace_back(Point2D(current_x, m_position.y), Point2D(current_x, m_position.y), Point2D(current_x, m_position.y));
        anchors.emplace_back(Point2D(current_x + char_w, m_position.y), Point2D(current_x + char_w, m_position.y), Point2D(current_x + char_w, m_position.y));
        anchors.emplace_back(Point2D(current_x + char_w, m_position.y + char_h), Point2D(current_x + char_w, m_position.y + char_h), Point2D(current_x + char_w, m_position.y + char_h));
        anchors.emplace_back(Point2D(current_x, m_position.y + char_h), Point2D(current_x, m_position.y + char_h), Point2D(current_x, m_position.y + char_h));

        current_x += char_w + tracking;
    }

    return std::make_unique<PathNode>(anchors);
}

void TextNode::bindToPath(const PathNode& target_path) {
    const auto& anchors = target_path.getAnchors();
    if (anchors.size() < 2) return;

    float s = 0.0f;
    for (char c : text_buffer) {
        (void)c;
        float char_w = font_size * 0.5f;
        float center_s = s + char_w * 0.5f;

        float t = center_s / 100.0f;
        if (t > 1.0f) t = 1.0f;

        const auto& p0 = anchors[0].position;
        const auto& p1 = anchors[0].handleOut;
        const auto& p2 = anchors[1].handleIn;
        const auto& p3 = anchors[1].position;

        float t1 = 1.0f - t;
        float x = t1*t1*t1*p0.x + 3*t1*t1*t*p1.x + 3*t1*t*t*p2.x + t*t*t*p3.x;
        float y = t1*t1*t1*p0.y + 3*t1*t1*t*p1.y + 3*t1*t*t*p2.y + t*t*t*p3.y;

        float tx = 3*t1*t1*(p1.x - p0.x) + 6*t1*t*(p2.x - p1.x) + 3*t*t*(p3.x - p2.x);
        float ty = 3*t1*t1*(p1.y - p0.y) + 6*t1*t*(p2.y - p1.y) + 3*t*t*(p3.y - p2.y);

        float angle = std::atan2(ty, tx);
        (void)angle; (void)x; (void)y;

        s += char_w + tracking;
    }
}

std::unique_ptr<CanvasNode> TextNode::clone() const {
    auto copy = std::make_unique<TextNode>(getText(), getX(), getY(), getFontSize());
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}
std::string TextNode::toSVG() const {
    return "<text x=\"" + std::to_string(m_position.x) + "\" y=\"" + std::to_string(m_position.y) +
           "\" font-size=\"" + std::to_string(font_size) + "\">" + text_buffer + "</text>";
}

} // namespace vectma
