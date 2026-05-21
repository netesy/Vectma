#include "renderer/RenderPipeline.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/modifiers/DashGeneratorModifier.hpp"

#ifdef VECTMA_USE_OPENGL
#include <GL/glew.h>
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

    // Search for DashGeneratorModifier in the stack to apply legacy stroke style if needed
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
    if (data.anchors.size() < 2) return;
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
#ifdef VECTMA_USE_OPENGL
    glEnable(GL_SCISSOR_TEST);
    glScissor((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
#else
    (void)rect;
#endif
}

void BaselineRenderer::popClipRect() {
    if (m_clipStack.empty()) return;
    m_clipStack.pop_back();
    if (m_clipStack.empty()) {
#ifdef VECTMA_USE_OPENGL
        glDisable(GL_SCISSOR_TEST);
#endif
    } else {
#ifdef VECTMA_USE_OPENGL
        const auto& rect = m_clipStack.back();
        glScissor((int)rect.x, (int)rect.y, (int)rect.width, (int)rect.height);
#endif
    }
}

void BaselineRenderer::setGlobalOpacity(float opacity) {
    m_globalOpacity = opacity;
}

} // namespace vectma
