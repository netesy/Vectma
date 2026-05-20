#pragma once

#include <string>
#include "core/GPoint.hpp"
#include "core/GColor.hpp"

namespace vectma {

class CanvasNode;
class RectNode;
class EllipseNode;
class PathNode;

/**
 * @brief Graphics hardware abstraction interface.
 * Replaces the legacy GPaintCanvas.
 */
class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    // Double-dispatch primitive hooks with styling parameters
    virtual void drawRect(const RectNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) = 0;
    virtual void drawEllipse(const EllipseNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) = 0;
    virtual void drawPath(const PathNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) = 0;

    // Bezier specific rendering
    virtual void drawBezierPath(const PathNode& node) = 0;
    virtual void drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) = 0;

    // Legacy support (to be phased out)
    virtual void renderNode(const CanvasNode& node) = 0;
};

/**
 * @brief Software/Null implementation of RenderPipeline for baseline.
 */
class BaselineRenderer : public RenderPipeline {
public:
    void beginFrame() override;
    void endFrame() override;

    void drawRect(const RectNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawEllipse(const EllipseNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawPath(const PathNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;

    void drawBezierPath(const PathNode& node) override;
    void drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) override;

    void renderNode(const CanvasNode& node) override;
};

} // namespace vectma
