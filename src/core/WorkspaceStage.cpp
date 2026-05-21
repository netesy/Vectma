#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/GeometryEngine.hpp"
#include "core/Commands.hpp"
#include "core/snap/SnappingEngine.hpp"
#include <algorithm>
#include <cmath>

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
    m_activeAnchorIndex = -1;
    m_activeHandleId = -1;
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

void WorkspaceStage::applyBooleanOperation(BooleanOp op) {
    (void)op;
    if (m_selection.size() < 2) return;
}

void WorkspaceStage::handleMouseDown(const Point2D& screenPos) {
    m_isDragging = true;
    m_dragStart = screenToCanvas(screenPos);
    m_cursorCanvasPos = m_dragStart;

    if (m_tool == ToolType::Rect) {
        auto rect = std::make_unique<RectNode>(m_dragStart.x, m_dragStart.y, 0, 0);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(rect)));
    }
}

void WorkspaceStage::handleMouseMove(const Point2D& screenPos) {
    m_cursorCanvasPos = screenToCanvas(screenPos);
    if (!m_isDragging) return;

    if (m_tool == ToolType::Marquee) {
        updateMarquee(m_cursorCanvasPos);
    }
}

void WorkspaceStage::handleMouseUp() {
    if (m_tool == ToolType::Marquee) {
        performSelection(m_marqueeRect);
    }
    m_isDragging = false;
    m_activeSnap = std::nullopt;
}

void WorkspaceStage::updateMarquee(const Point2D& currentCanvasPos) {
    double x = std::min(m_dragStart.x, currentCanvasPos.x);
    double y = std::min(m_dragStart.y, currentCanvasPos.y);
    double w = std::abs(m_dragStart.x - currentCanvasPos.x);
    double h = std::abs(m_dragStart.y - currentCanvasPos.y);
    m_marqueeRect = GRect(x, y, w, h);
}

void WorkspaceStage::performSelection(const GRect& rect) {
    clearSelection();
    if (!m_scene) return;
    auto hits = m_scene->queryVisible(rect);
    for (auto node : hits) {
        addToSelection(node);
    }
}

size_t WorkspaceStage::getSceneNodeCount() const {
    if (!m_scene) return 0;
    return countNodesRecursive(m_scene.get());
}

size_t WorkspaceStage::countNodesRecursive(const CanvasNode* node) const {
    size_t count = 1;
    for (const auto& child : node->getChildren()) {
        count += countNodesRecursive(child.get());
    }
    return count;
}

} // namespace vectma
