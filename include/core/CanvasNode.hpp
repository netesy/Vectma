#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/GPoint.hpp"
#include "core/GRect.hpp"
#include "core/GColor.hpp"

namespace vectma {

class RenderPipeline;

/**
 * @brief Abstract polymorphic base class for all scene objects.
 * Replaces the legacy GObject.
 */
class CanvasNode {
public:
    CanvasNode();
    virtual ~CanvasNode();

    // Disable copying for unique ownership management
    CanvasNode(const CanvasNode&) = delete;
    CanvasNode& operator=(const CanvasNode&) = delete;

    virtual std::string getClassName() const = 0;

    void setParent(CanvasNode* parent);
    CanvasNode* getParent() const;

    void addChild(std::unique_ptr<CanvasNode> child);
    const std::vector<std::unique_ptr<CanvasNode>>& getChildren() const;

    // Visibility
    bool isVisible() const { return m_visible; }
    void setVisibility(bool visible) { m_visible = visible; }

    // Styling Properties
    FillType getFillType() const { return m_fillType; }
    void setFillType(FillType type) { m_fillType = type; }

    const GradientConfig& getGradientConfig() const { return m_gradientConfig; }
    void setGradientConfig(const GradientConfig& config) { m_gradientConfig = config; }

    GColor getFillColor() const { return m_fillColor; }
    void setFillColor(GColor color) { m_fillColor = color; }

    StrokeAlignment getStrokeAlignment() const { return m_strokeAlignment; }
    void setStrokeAlignment(StrokeAlignment alignment) { m_strokeAlignment = alignment; }

    double getStrokeWidth() const { return m_strokeWidth; }
    void setStrokeWidth(double width) { m_strokeWidth = width; }

    // Dispatch Anchor for Visitor Pattern
    virtual void render(RenderPipeline& pipeline) const = 0;

    // Geometric Interfaces
    virtual bool containsPoint(const GPoint& point) const = 0;
    virtual GRect computeBoundingBox() const = 0;

    // Phase 10: Asset Export
    virtual std::string toSVG() const = 0;

protected:
    CanvasNode* m_parent = nullptr;
    std::vector<std::unique_ptr<CanvasNode>> m_children;
    bool m_visible = true;

    // Style data
    FillType m_fillType = FillType::Solid;
    GradientConfig m_gradientConfig;
    GColor m_fillColor = GColor::White();
    StrokeAlignment m_strokeAlignment = StrokeAlignment::Center;
    double m_strokeWidth = 1.0;
};

} // namespace vectma
