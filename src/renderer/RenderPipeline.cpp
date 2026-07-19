#include "renderer/RenderPipeline.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/ComponentInstanceNode.hpp"
#include "core/MasterComponentNode.hpp"
#include "core/LayerNode.hpp"
#include "core/ArtboardNode.hpp"
#include "core/SceneGraph.hpp"
#include "core/modifiers/DashGeneratorModifier.hpp"
#include "core/LinearAllocator.hpp"

#ifdef VECTMA_USE_SKIA
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#endif

#include <iostream>
#include <cmath>

namespace vectma {

void BaselineRenderer::beginFrame() {
    m_opacityStack = {1.0f};
    m_clipStack.clear();
    m_transformStack.clear();
    // Reclaim all scratchpad arena memory at the start of each frame
    LinearAllocator::getThreadLocal().reset();
}
void BaselineRenderer::endFrame() {}

void BaselineRenderer::pushTransform(const GTransform& transform) { m_transformStack.push_back(transform); }
void BaselineRenderer::popTransform() { if (!m_transformStack.empty()) m_transformStack.pop_back(); }

void BaselineRenderer::drawRect(const RectNode& node, FillType, const GradientConfig&, StrokeAlignment) { (void)node; }
void BaselineRenderer::drawEllipse(const EllipseNode& node, FillType, const GradientConfig&, StrokeAlignment) { (void)node; }
void BaselineRenderer::drawPath(const PathNode& node, FillType, const GradientConfig&, StrokeAlignment) { drawBezierPath(node); }
void BaselineRenderer::drawText(const TextNode& node) { (void)node; }
void BaselineRenderer::drawImage(const ImageNode& node) { (void)node; }
void BaselineRenderer::drawCompoundShape(const CompoundShapeNode& node) {
    for (const auto& child : node.getChildren()) {
        if (child->isVisible()) renderNode(*child);
    }
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
#endif
}

void BaselineRenderer::drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) { (void)anchor; (void)selected; (void)activeHandle; }
void BaselineRenderer::renderNode(const CanvasNode& node) {
    pushOpacity(node.getOpacity());
    switch (node.getNodeType()) {
        case NodeType::Rect:
            static_cast<const RectNode&>(node).render(*this);
            break;
        case NodeType::Ellipse:
            static_cast<const EllipseNode&>(node).render(*this);
            break;
        case NodeType::Path:
            static_cast<const PathNode&>(node).render(*this);
            break;
        case NodeType::Text:
            static_cast<const TextNode&>(node).render(*this);
            break;
        case NodeType::Image:
            static_cast<const ImageNode&>(node).render(*this);
            break;
        case NodeType::Compound:
            static_cast<const CompoundShapeNode&>(node).render(*this);
            break;
        case NodeType::ComponentInstance:
            static_cast<const ComponentInstanceNode&>(node).render(*this);
            break;
        case NodeType::MasterComponent:
            static_cast<const MasterComponentNode&>(node).render(*this);
            break;
        case NodeType::Layer:
            static_cast<const LayerNode&>(node).render(*this);
            break;
        case NodeType::Artboard:
            static_cast<const ArtboardNode&>(node).render(*this);
            break;
        case NodeType::SceneGraph:
            static_cast<const SceneGraph&>(node).render(*this);
            break;
        default:
            node.render(*this);
            break;
    }
    popOpacity();
}
std::vector<uint8_t> BaselineRenderer::exportRaster(float) { return {}; }
void BaselineRenderer::setStrokeStyle(const std::vector<float>&, float) {}
void BaselineRenderer::drawSnappingGuide(const Point2D&, const Point2D&) {}
void BaselineRenderer::drawAlignmentGuide(const Point2D&, const Point2D&) {}
void BaselineRenderer::pushClipRect(const GRect& rect) { m_clipStack.push_back(rect); }
void BaselineRenderer::popClipRect() { if (!m_clipStack.empty()) m_clipStack.pop_back(); }
void BaselineRenderer::pushOverrideContext(const OverrideMap* overrides) { (void)overrides; }
void BaselineRenderer::popOverrideContext() {}
void BaselineRenderer::setGlobalOpacity(float opacity) { m_opacityStack.back() = opacity; }
void BaselineRenderer::pushOpacity(float opacity) { m_opacityStack.push_back(m_opacityStack.back() * opacity); }
void BaselineRenderer::popOpacity() { if (m_opacityStack.size() > 1) m_opacityStack.pop_back(); }

void BaselineRenderer::drawInteractionWire(const Point2D& start, const Point2D& end) { (void)start; (void)end; }

void BaselineRenderer::drawTransition(const CanvasNode& outgoing, const CanvasNode& incoming, TransitionType type, float progress, double width, double height) {
    (void)outgoing;
    (void)type; (void)progress; (void)width; (void)height;
    incoming.render(*this);
}

} // namespace vectma
