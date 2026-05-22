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

    float getX() const { return m_position.value.x; }
    void setX(float x) { m_position.update({x, getY()}, LamportClock::getInstance().tick()); }
    float getY() const { return m_position.value.y; }
    void setY(float y) { m_position.update({getX(), y}, LamportClock::getInstance().tick()); }

    void setXRemote(float x, LamportTimestamp ts) { m_position.update({x, getY()}, ts); }
    void setYRemote(float y, LamportTimestamp ts) { m_position.update({getX(), y}, ts); }

    float getFontSize() const { return m_fontSize.value; }
    void setFontSize(float size) { m_fontSize.update(size, LamportClock::getInstance().tick()); }
    void setFontSizeRemote(float size, LamportTimestamp ts) { m_fontSize.update(size, ts); }

    std::string getText() const { return m_textBuffer.value; }
    void setText(const std::string& text) { m_textBuffer.update(text, LamportClock::getInstance().tick()); }
    void setTextRemote(const std::string& text, LamportTimestamp ts) { m_textBuffer.update(text, ts); }

    std::string getFontFamily() const { return m_fontFamily.value; }
    void setFontFamily(const std::string& family) { m_fontFamily.update(family, LamportClock::getInstance().tick()); }
    void setFontFamilyRemote(const std::string& family, LamportTimestamp ts) { m_fontFamily.update(family, ts); }

    float getTracking() const { return m_tracking.value; }
    void setTracking(float t) { m_tracking.update(t, LamportClock::getInstance().tick()); }

    float getLeading() const { return m_leading.value; }
    void setLeading(float l) { m_leading.update(l, LamportClock::getInstance().tick()); }

    std::unique_ptr<PathNode> toPathNode() const;
    void bindToPath(const PathNode& target_path);

private:
    LWWProperty<GPoint> m_position;
    LWWProperty<std::string> m_textBuffer;
    LWWProperty<std::string> m_fontFamily;
    LWWProperty<float> m_fontSize;
    LWWProperty<float> m_tracking;
    LWWProperty<float> m_leading;
};

} // namespace vectma
