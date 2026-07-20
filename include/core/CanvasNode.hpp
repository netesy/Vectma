#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include "core/GPoint.hpp"
#include "core/GRect.hpp"
#include "core/GColor.hpp"
#include "core/Geometry.hpp"
#include "sync/CRDTTypes.hpp"
#include "layout/LayoutSolver.hpp"
#include "style/TokenRegistry.hpp"

namespace vectma {

class RenderPipeline;

enum class NodeType {
    Base,
    SceneGraph,
    Rect,
    Ellipse,
    Path,
    Text,
    Image,
    Compound,
    ComponentInstance,
    SymbolInstance,
    MasterComponent,
    Layer,
    Artboard
};

class CanvasNode {
public:
    CanvasNode();
    virtual ~CanvasNode();

    NodeType getNodeType() const { return m_nodeType; }

    CanvasNode(const CanvasNode&) = delete;
    CanvasNode& operator=(const CanvasNode&) = delete;

    virtual std::string getClassName() const = 0;

    void setParent(CanvasNode* parent);
    CanvasNode* getParent() const;

    virtual void addChild(std::unique_ptr<CanvasNode> child);
    virtual std::unique_ptr<CanvasNode> removeChild(CanvasNode* node);
    const std::vector<std::unique_ptr<CanvasNode>>& getChildren() const;
    std::vector<std::unique_ptr<CanvasNode>>& getChildrenMutable();

    NodeId getId() const { return m_id.value; }
    void setId(NodeId id) { m_id.update(id, LamportClock::getInstance().tick()); }
    void setIdRemote(NodeId id, LamportTimestamp ts) { m_id.update(id, ts); }

    bool isVisible() const { return m_visible.value; }
    void setVisibility(bool visible) { m_visible.update(visible, LamportClock::getInstance().tick()); }
    void setVisibilityRemote(bool visible, LamportTimestamp ts) { m_visible.update(visible, ts); }

    bool isLocked() const { return m_locked.value; }
    void setLocked(bool locked) { m_locked.update(locked, LamportClock::getInstance().tick()); }
    void setLockedRemote(bool locked, LamportTimestamp ts) { m_locked.update(locked, ts); }

    float getOpacity() const { return m_opacity.value; }
    void setOpacity(float opacity) { m_opacity.update(std::clamp(opacity, 0.0f, 1.0f), LamportClock::getInstance().tick()); }
    void setOpacityRemote(float opacity, LamportTimestamp ts) { m_opacity.update(std::clamp(opacity, 0.0f, 1.0f), ts); }

    BlendMode getBlendMode() const { return m_blendMode.value; }
    void setBlendMode(BlendMode mode) { m_blendMode.update(mode, LamportClock::getInstance().tick()); }
    void setBlendModeRemote(BlendMode mode, LamportTimestamp ts) { m_blendMode.update(mode, ts); }

    FillType getFillType() const { return m_fillType.value; }
    void setFillType(FillType type) { m_fillType.update(type, LamportClock::getInstance().tick()); }
    void setFillTypeRemote(FillType type, LamportTimestamp ts) { m_fillType.update(type, ts); }

    const GradientConfig& getGradientConfig() const { return m_gradientConfig.value; }
    void setGradientConfig(const GradientConfig& config) { m_gradientConfig.update(config, LamportClock::getInstance().tick()); }
    void setGradientConfigRemote(const GradientConfig& config, LamportTimestamp ts) { m_gradientConfig.update(config, ts); }

    GColor getFillColor() const {
        if (m_fillColor.tokenPath) return TokenRegistry::getInstance().resolveAs<GColor>(*m_fillColor.tokenPath);
        return m_fillColor.value;
    }
    void setFillColor(GColor color) { m_fillColor.update(color, LamportClock::getInstance().tick()); }
    void setFillColorRemote(GColor color, LamportTimestamp ts) { m_fillColor.update(color, ts); }
    void setFillColorToken(const std::string& path) { m_fillColor.bindToken(path, LamportClock::getInstance().tick()); }

    StrokeAlignment getStrokeAlignment() const { return m_strokeAlignment.value; }
    void setStrokeAlignment(StrokeAlignment alignment) { m_strokeAlignment.update(alignment, LamportClock::getInstance().tick()); }
    void setStrokeAlignmentRemote(StrokeAlignment alignment, LamportTimestamp ts) { m_strokeAlignment.update(alignment, ts); }

    double getStrokeWidth() const {
        if (m_strokeWidth.tokenPath) return TokenRegistry::getInstance().resolveAs<double>(*m_strokeWidth.tokenPath);
        return m_strokeWidth.value;
    }
    void setStrokeWidth(double width) { m_strokeWidth.update(width, LamportClock::getInstance().tick()); markLayoutDirty(); }
    void setStrokeWidthRemote(double width, LamportTimestamp ts) { m_strokeWidth.update(width, ts); markLayoutDirty(); }
    void setStrokeWidthToken(const std::string& path) { m_strokeWidth.bindToken(path, LamportClock::getInstance().tick()); markLayoutDirty(); }

    const LayoutProperties& getLayoutProps() const { return m_layoutProps; }
    void setLayoutProps(const LayoutProperties& props) { m_layoutProps = props; markLayoutDirty(); }

    SizingRule getHorizontalSizing() const { return m_hSizing; }
    void setHorizontalSizing(SizingRule rule) { m_hSizing = rule; markLayoutDirty(); }

    SizingRule getVerticalSizing() const { return m_vSizing; }
    void setVerticalSizing(SizingRule rule) { m_vSizing = rule; markLayoutDirty(); }

    static constexpr uint32_t DIRTY_NONE      = 0;
    static constexpr uint32_t DIRTY_GEOMETRY  = 1 << 0;
    static constexpr uint32_t DIRTY_STYLING   = 1 << 1;
    static constexpr uint32_t DIRTY_LAYOUT    = 1 << 2;
    static constexpr uint32_t DIRTY_SUBTREE   = 1 << 3;
    static constexpr uint32_t DIRTY_ALL       = 0xFFFFFFFF;

    void markLayoutDirty();
    bool isLayoutDirty() const { return (m_dirtyMask & DIRTY_LAYOUT) != 0; }
    void clearLayoutDirty() { m_dirtyMask &= ~DIRTY_LAYOUT; }

    uint32_t getDirtyMask() const { return m_dirtyMask; }
    void addDirtyFlags(uint32_t flags) { m_dirtyMask |= flags; }
    void clearDirtyFlags(uint32_t flags) { m_dirtyMask &= ~flags; }
    bool hasDirtyFlags(uint32_t flags) const { return (m_dirtyMask & flags) != 0; }

    virtual double getX() const { return 0; }
    virtual double getY() const { return 0; }
    virtual double getWidth() const { return 0; }
    virtual double getHeight() const { return 0; }
    virtual void setPositionRemote(double x, double y, LamportTimestamp ts) { (void)x; (void)y; (void)ts; }
    virtual void setSizeRemote(double w, double h, LamportTimestamp ts) { (void)w; (void)h; (void)ts; }

    float m_prefWidth = 0;
    float m_prefHeight = 0;

    virtual void render(RenderPipeline& pipeline) const = 0;
    virtual bool containsPoint(const GPoint& point) const = 0;
    virtual GRect computeBoundingBox() const = 0;
    virtual std::string toSVG() const = 0;
    virtual std::unique_ptr<CanvasNode> clone() const = 0;

    void bringToFront();
    void sendToBack();
    void raiseNode();
    void lowerNode();

    static void CloneBaseProperties(const CanvasNode& src, CanvasNode& dst);

protected:
    NodeType m_nodeType = NodeType::Base;
    CanvasNode* m_parent = nullptr;
    std::vector<std::unique_ptr<CanvasNode>> m_children;
    LWWProperty<NodeId> m_id;
    LWWProperty<bool> m_visible;
    LWWProperty<bool> m_locked;
    LWWProperty<float> m_opacity;
    LWWProperty<BlendMode> m_blendMode;
    LWWProperty<FillType> m_fillType;
    LWWProperty<GradientConfig> m_gradientConfig;
    LWWProperty<GColor> m_fillColor;
    LWWProperty<StrokeAlignment> m_strokeAlignment;
    LWWProperty<double> m_strokeWidth;

    LayoutProperties m_layoutProps;
    SizingRule m_hSizing = SizingRule::Fixed;
    SizingRule m_vSizing = SizingRule::Fixed;
    uint32_t m_dirtyMask = DIRTY_ALL;
};

} // namespace vectma
