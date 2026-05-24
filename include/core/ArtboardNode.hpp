#pragma once
#include "core/CanvasNode.hpp"
#include <vector>
#include <string>

namespace vectma {

struct DevicePreset {
    std::string name;
    int width;
    int height;
    std::string category;
};

class ArtboardNode : public CanvasNode {
public:
    ArtboardNode(const std::string& name = "Artboard", const GRect& bounds = {0, 0, 800, 600});

    std::string getClassName() const override { return "ArtboardNode"; }

    const std::string& getName() const { return m_name.value; }
    void setName(const std::string& name) { m_name.update(name, LamportClock::getInstance().tick()); }

    GRect getBounds() const { return m_bounds.value; }
    void setBounds(const GRect& bounds) { m_bounds.update(bounds, LamportClock::getInstance().tick()); markLayoutDirty(); }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;
    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    double getX() const override { return m_bounds.value.x; }
    double getY() const override { return m_bounds.value.y; }
    double getWidth() const override { return m_bounds.value.width; }
    double getHeight() const override { return m_bounds.value.height; }

    void setPositionRemote(double x, double y, LamportTimestamp ts) override {
        GRect b = m_bounds.value;
        b.x = x; b.y = y;
        m_bounds.update(b, ts);
        markLayoutDirty();
    }
    void setSizeRemote(double w, double h, LamportTimestamp ts) override {
        GRect b = m_bounds.value;
        b.width = w; b.height = h;
        m_bounds.update(b, ts);
        markLayoutDirty();
    }

    static const std::vector<DevicePreset>& GetPresets() {
        static const std::vector<DevicePreset> presets = {
            {"Desktop HD", 1440, 900, "Desktop"},
            {"Desktop FHD", 1920, 1080, "Desktop"},
            {"iPhone 13/14", 390, 844, "Mobile"},
            {"Pixel 7", 412, 915, "Mobile"},
            {"iPad Pro", 834, 1194, "Tablet"}
        };
        return presets;
    }

private:
    LWWProperty<std::string> m_name;
    LWWProperty<GRect> m_bounds;
};

} // namespace vectma
