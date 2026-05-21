#include "renderer/RenderPipeline.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/modifiers/DashGeneratorModifier.hpp"

#ifdef VECTMA_USE_OPENGL
#include <GL/glew.h>
#endif

#ifdef VECTMA_USE_SKIA
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#endif

#include <iostream>
#include <cmath>

namespace vectma {

void BaselineRenderer::beginFrame() {
    m_globalOpacity = 1.0f;
    m_clipStack.clear();
}

void BaselineRenderer::endFrame() {
}

void BaselineRenderer::drawRect(const RectNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) {
    (void)fillType; (void)gradConfig; (void)strokeAlign; (void)node;
}

void BaselineRenderer::drawEllipse(const EllipseNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) {
    (void)fillType; (void)gradConfig; (void)strokeAlign; (void)node;
}

void BaselineRenderer::drawPath(const PathNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) {
    (void)fillType; (void)gradConfig; (void)strokeAlign;

    for (const auto& mod : node.getModifierStack()) {
        if (auto dashMod = dynamic_cast<DashGeneratorModifier*>(mod.get())) {
            setStrokeStyle(dashMod->getPattern(), dashMod->getOffset());
            break;
        }
    }

    drawBezierPath(node);
}

void BaselineRenderer::drawText(const TextNode& node) {
    (void)node;
}

void BaselineRenderer::drawBezierPath(const PathNode& node) {
    const auto& data = node.getCompiledPath();
    if (data.contours.empty()) return;

#ifdef VECTMA_USE_SKIA
    SkPath path;
    for (const auto& contour : data.contours) {
        if (contour.anchors.empty()) continue;

        const auto& anchors = contour.anchors;
        path.moveTo(anchors[0].position.x, anchors[0].position.y);
        for (size_t i = 0; i < anchors.size() - 1; ++i) {
            path.cubicTo(anchors[i].handleOut.x, anchors[i].handleOut.y,
                         anchors[i+1].handleIn.x, anchors[i+1].handleIn.y,
                         anchors[i+1].position.x, anchors[i+1].position.y);
        }
        if (contour.isClosed && anchors.size() > 1) {
            path.cubicTo(anchors.back().handleOut.x, anchors.back().handleOut.y,
                         anchors.front().handleIn.x, anchors.front().handleIn.y,
                         anchors.front().position.x, anchors.front().position.y);
            path.close();
        }
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    // m_skCanvas->drawPath(path, paint);
#endif
}

void BaselineRenderer::drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) {
    (void)anchor; (void)selected; (void)activeHandle;
}

void BaselineRenderer::renderNode(const CanvasNode& node) {
    float oldOpacity = m_globalOpacity;
    m_globalOpacity *= node.getOpacity();

    node.render(*this);

    m_globalOpacity = oldOpacity;
}

std::vector<uint8_t> BaselineRenderer::exportRaster(float scale) {
    (void)scale;
    return std::vector<uint8_t>();
}

void BaselineRenderer::setStrokeStyle(const std::vector<float>& dashPattern, float offset) {
    (void)dashPattern; (void)offset;
}

void BaselineRenderer::drawSnappingGuide(const Point2D& start, const Point2D& end) {
    (void)start; (void)end;
}

void BaselineRenderer::pushClipRect(const GRect& rect) {
    m_clipStack.push_back(rect);
}

void BaselineRenderer::popClipRect() {
    if (m_clipStack.empty()) return;
    m_clipStack.pop_back();
}

void BaselineRenderer::setGlobalOpacity(float opacity) {
    m_globalOpacity = opacity;
}

} // namespace vectma
