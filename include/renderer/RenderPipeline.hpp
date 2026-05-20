#pragma once

#include <string>
#include <vector>
#include "core/GPoint.hpp"
#include "core/GColor.hpp"

namespace vectma {

class CanvasNode;
class RectNode;
class EllipseNode;
class PathNode;
class TextNode;

/**
 * @brief Graphics hardware abstraction interface.
 */
class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    virtual void drawRect(const RectNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) = 0;
    virtual void drawEllipse(const EllipseNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) = 0;
    virtual void drawPath(const PathNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) = 0;
    virtual void drawText(const TextNode& node) = 0;

    virtual void drawBezierPath(const PathNode& node) = 0;
    virtual void drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) = 0;

    virtual void renderNode(const CanvasNode& node) = 0;

    // Phase 10: Offscreen Raster Export
    virtual std::vector<uint8_t> exportRaster(float scale) = 0;
};

class BaselineRenderer : public RenderPipeline {
public:
    void beginFrame() override;
    void endFrame() override;

    void drawRect(const RectNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawEllipse(const EllipseNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawPath(const PathNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawText(const TextNode& node) override;

    void drawBezierPath(const PathNode& node) override;
    void drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) override;

    void renderNode(const CanvasNode& node) override;

    std::vector<uint8_t> exportRaster(float scale) override;
};

} // namespace vectma
