#pragma once

#include "renderer/RenderPipeline.hpp"
#include "core/PathData.hpp"
#include "core/BrushTypes.hpp"

#ifdef VECTMA_USE_SKIA
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"
#include "include/core/SkPaint.h"
#endif

#include <memory>
#include <vector>

namespace vectma {

#ifdef VECTMA_USE_SKIA
class SkiaRenderPipeline : public RenderPipeline {
public:
    SkiaRenderPipeline(SkCanvas* canvas);
    ~SkiaRenderPipeline() override = default;

    void beginFrame() override;
    void endFrame() override;

    void drawRect(const RectNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawEllipse(const EllipseNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawPath(const PathNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) override;
    void drawText(const TextNode& node) override;
    void drawImage(const class ImageNode& node) override;
    void drawCompoundShape(const class CompoundShapeNode& node) override;

    void drawBezierPath(const PathNode& node) override;
    void drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) override;

    void renderNode(const CanvasNode& node) override;

    std::vector<uint8_t> exportRaster(float scale) override;

    void setStrokeStyle(const std::vector<float>& dashPattern, float offset) override;

    void drawSnappingGuide(const Point2D& start, const Point2D& end) override;

    void pushClipRect(const GRect& rect) override;
    void popClipRect() override;
    void setGlobalOpacity(float opacity) override;

    // Advanced Extensions
    void drawBrushStroke(const std::vector<BrushPoint>& strokeBuffer, const BrushSettings& settings);

private:
    SkPath TranslateToSkPath(const PathData& nativePath);

#ifdef VECTMA_USE_SKIA
    sk_sp<SkRuntimeEffect> m_brushEffect;
#endif
    SkCanvas* m_canvas;
    float m_globalOpacity = 1.0f;
};
#endif

} // namespace vectma
