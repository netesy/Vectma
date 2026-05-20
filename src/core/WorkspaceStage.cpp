#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include <algorithm>

namespace vectma {

WorkspaceStage::WorkspaceStage() = default;

WorkspaceStage::~WorkspaceStage() = default;

void WorkspaceStage::setScene(std::shared_ptr<SceneGraph> scene) {
    m_scene = std::move(scene);
}

std::shared_ptr<SceneGraph> WorkspaceStage::getScene() const {
    return m_scene;
}

void WorkspaceStage::addToSelection(CanvasNode* node) {
    m_selection.push_back(node);
}

void WorkspaceStage::clearSelection() {
    m_selection.clear();
}

const std::vector<CanvasNode*>& WorkspaceStage::getSelection() const {
    return m_selection;
}

void WorkspaceStage::setTool(ToolType tool) {
    m_tool = tool;
    m_isDragging = false;
    clearSelection();
}

GPoint WorkspaceStage::screenToCanvas(const GPoint& screenPos) const {
    return m_viewMatrix.inverse().map(screenPos);
}

void WorkspaceStage::handleMouseDown(const GPoint& screenPos) {
    m_isDragging = true;
    m_dragStart = screenToCanvas(screenPos);
    m_marqueeRect = GRect(m_dragStart.x, m_dragStart.y, 0, 0);
}

void WorkspaceStage::handleMouseMove(const GPoint& screenPos) {
    if (!m_isDragging) return;

    GPoint canvasPos = screenToCanvas(screenPos);
    updateMarquee(canvasPos);
}

void WorkspaceStage::handleMouseUp() {
    if (!m_isDragging) return;
    m_isDragging = false;

    if (!m_scene) return;

    if (m_tool == ToolType::Marquee) {
        performSelection(m_marqueeRect);
    } else if (m_tool == ToolType::Rect) {
        if (m_marqueeRect.width > 0 && m_marqueeRect.height > 0) {
            m_scene->addChild(std::make_unique<RectNode>(m_marqueeRect.x, m_marqueeRect.y, m_marqueeRect.width, m_marqueeRect.height));
        }
    } else if (m_tool == ToolType::Ellipse) {
        if (m_marqueeRect.width > 0 && m_marqueeRect.height > 0) {
            double rx = m_marqueeRect.width / 2.0;
            double ry = m_marqueeRect.height / 2.0;
            m_scene->addChild(std::make_unique<EllipseNode>(m_marqueeRect.x + rx, m_marqueeRect.y + ry, rx, ry));
        }
    }
}

void WorkspaceStage::updateMarquee(const GPoint& currentCanvasPos) {
    double x = std::min(m_dragStart.x, currentCanvasPos.x);
    double y = std::min(m_dragStart.y, currentCanvasPos.y);
    double w = std::abs(m_dragStart.x - currentCanvasPos.x);
    double h = std::abs(m_dragStart.y - currentCanvasPos.y);
    m_marqueeRect = GRect(x, y, w, h);
}

void WorkspaceStage::performSelection(const GRect& rect) {
    clearSelection();
    if (!m_scene) return;

    // Spatial hit-testing against scene nodes
    for (const auto& child : m_scene->getChildren()) {
        GRect bbox = child->computeBoundingBox();
        // Simple intersection check: if bbox is within marquee
        if (bbox.x >= rect.x && bbox.x + bbox.width <= rect.x + rect.width &&
            bbox.y >= rect.y && bbox.y + bbox.height <= rect.y + rect.height) {
            addToSelection(child.get());
        }
    }
}

} // namespace vectma
