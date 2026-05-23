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

namespace vectma {

class RenderPipeline;

/**
 * @brief Abstract polymorphic base class for all scene objects.
 * Upgraded with LWW-CRDT properties for collaborative sync.
 */
class CanvasNode {
public:
    CanvasNode();
    virtual ~CanvasNode();

    CanvasNode(const CanvasNode&) = delete;
    CanvasNode& operator=(const CanvasNode&) = delete;

    virtual std::string getClassName() const = 0;

    void setParent(CanvasNode* parent);
    CanvasNode* getParent() const;

    void addChild(std::unique_ptr<CanvasNode> child);
    const std::vector<std::unique_ptr<CanvasNode>>& getChildren() const;
    std::vector<std::unique_ptr<CanvasNode>>& getChildrenMutable();

    // CRDT Identity
    NodeId getId() const { return m_id.value; }
    void setId(NodeId id, LamportTimestamp ts) { m_id.update(id, ts); }

    // Visibility
    bool isVisible() const { return m_visible.value; }
    void setVisibility(bool visible, LamportTimestamp ts) { m_visible.update(visible, ts); }

    // Locking
    bool isLocked() const { return m_locked.value; }
    void setLocked(bool locked, LamportTimestamp ts) { m_locked.update(locked, ts); }

    // Opacity
    float getOpacity() const { return m_opacity.value; }
    void setOpacity(float opacity, LamportTimestamp ts) { m_opacity.update(std::clamp(opacity, 0.0f, 1.0f), ts); }

    BlendMode getBlendMode() const { return m_blendMode.value; }
    void setBlendMode(BlendMode mode, LamportTimestamp ts) { m_blendMode.update(mode, ts); }

    // Styling Properties
    FillType getFillType() const { return m_fillType.value; }
    void setFillType(FillType type, LamportTimestamp ts) { m_fillType.update(type, ts); }

    const GradientConfig& getGradientConfig() const { return m_gradientConfig.value; }
    void setGradientConfig(const GradientConfig& config, LamportTimestamp ts) { m_gradientConfig.update(config, ts); }

    GColor getFillColor() const { return m_fillColor.value; }
    void setFillColor(GColor color, LamportTimestamp ts) { m_fillColor.update(color, ts); }

    StrokeAlignment getStrokeAlignment() const { return m_strokeAlignment.value; }
    void setStrokeAlignment(StrokeAlignment alignment, LamportTimestamp ts) { m_strokeAlignment.update(alignment, ts); }

    double getStrokeWidth() const { return m_strokeWidth.value; }
    void setStrokeWidth(double width, LamportTimestamp ts) { m_strokeWidth.update(width, ts); }

    // Dispatch Anchor for Visitor Pattern
    virtual void render(RenderPipeline& pipeline) const = 0;

    // Geometric Interfaces
    virtual bool containsPoint(const GPoint& point) const = 0;
    virtual GRect computeBoundingBox() const = 0;

    virtual std::string toSVG() const = 0;

    // Z-Order Manipulation
    void bringToFront();
    void sendToBack();
    void raiseNode();
    void lowerNode();

protected:
    CanvasNode* m_parent = nullptr;
    std::vector<std::unique_ptr<CanvasNode>> m_children;

    // CRDT Backed Properties
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
};

} // namespace vectma
