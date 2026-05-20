#pragma once

#include "core/CanvasNode.hpp"
#include "core/FontAsset.hpp"
#include "core/PathNode.hpp"
#include <string>
#include <memory>

namespace vectma {

class TextNode : public CanvasNode {
public:
    TextNode(const std::string& text, std::shared_ptr<FontAsset> font, float size = 12.0f);

    std::string getClassName() const override { return "TextNode"; }
    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;

    // Text specific
    std::string text_buffer;
    std::shared_ptr<FontAsset> active_font;
    float font_size;
    float tracking = 0.0f;
    float leading = 1.0f;

    std::unique_ptr<PathNode> toPathNode() const;
    void bindToPath(const PathNode& target_path);

private:
    GPoint m_position;
};

} // namespace vectma
