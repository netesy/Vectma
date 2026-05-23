#pragma once

#include "core/CanvasNode.hpp"
#include "core/FontAsset.hpp"
#include "core/PathNode.hpp"
#include <string>
#include <memory>

namespace vectma {

class TextNode : public CanvasNode {
public:
    TextNode(const std::string& text, float x = 0, float y = 0, float size = 12.0f);

    std::string getClassName() const override { return "TextNode"; }
    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    // Text specific
    std::string text_buffer;
    std::string font_family = "Inter";
    float font_size;
    float tracking = 0.0f;
    float leading = 1.0f;

    float getX() const { return m_position.x; }
    void setX(float x) { m_position.x = x; }
    float getY() const { return m_position.y; }
    void setY(float y) { m_position.y = y; }

    float getFontSize() const { return font_size; }
    void setFontSize(float size) { font_size = size; }

    std::string getText() const { return text_buffer; }
    void setText(const std::string& text) { text_buffer = text; }

    std::unique_ptr<PathNode> toPathNode() const;
    void bindToPath(const PathNode& target_path);

private:
    GPoint m_position;
};

} // namespace vectma
