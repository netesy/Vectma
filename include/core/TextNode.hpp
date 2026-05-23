#pragma once
#include "core/CanvasNode.hpp"

namespace vectma {

class TextNode : public CanvasNode {
public:
    TextNode(const std::string& text, double x, double y);

    std::string getClassName() const override { return "TextNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    const std::string& getText() const { return m_text.value; }
    void setText(const std::string& text) { m_text.update(text, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setTextRemote(const std::string& text, LamportTimestamp ts) { m_text.update(text, ts); markLayoutDirty(); }

    double getX() const override { return m_x.value; }
    double getY() const override { return m_y.value; }
    void setX(double x) { m_x.update(x, LamportClock::getInstance().tick()); }
    void setY(double y) { m_y.update(y, LamportClock::getInstance().tick()); }

    void setPositionRemote(double x, double y, LamportTimestamp ts) override {
        m_x.update(x, ts);
        m_y.update(y, ts);
        markLayoutDirty();
    }

    float getFontSize() const { return m_fontSize.value; }
    void setFontSize(float size) { m_fontSize.update(size, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setFontSizeRemote(float size, LamportTimestamp ts) { m_fontSize.update(size, ts); markLayoutDirty(); }

private:
    LWWProperty<std::string> m_text;
    LWWProperty<double> m_x;
    LWWProperty<double> m_y;
    LWWProperty<float> m_fontSize;
};

} // namespace vectma
