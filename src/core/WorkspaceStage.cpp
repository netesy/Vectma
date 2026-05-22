#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/GeometryEngine.hpp"
#include "core/Commands.hpp"
#include "core/snap/SnappingEngine.hpp"
#include <algorithm>
#include <cmath>

namespace vectma {

WorkspaceStage::WorkspaceStage() {
    m_viewMatrix = GTransform::Identity();
}

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
    m_activeStroke.clear();
    clearSelection();
}

GPoint WorkspaceStage::screenToCanvas(const GPoint& screenPos) const {
    return m_viewMatrix.inverse().map(screenPos);
}

void WorkspaceStage::applyBooleanOperation(BooleanOp op) {
    if (m_selection.size() < 2) return;
    PathNode* target = dynamic_cast<PathNode*>(m_selection[0]);
    PathNode* source = dynamic_cast<PathNode*>(m_selection[1]);
    if (target && source) {
        auto result = GeometryEngine::combinePaths(*target, *source, op);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(result)));
    }
}

void WorkspaceStage::handleMouseDown(const Point2D& screenPos) {
    m_isDragging = true;
    m_dragStart = screenToCanvas(screenPos);
    m_cursorCanvasPos = m_dragStart;

    if (m_tool == ToolType::Rect) {
        auto rect = std::make_unique<RectNode>(m_dragStart.x, m_dragStart.y, 0, 0);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(rect)));
    } else if (m_tool == ToolType::Ellipse) {
        auto ellipse = std::make_unique<EllipseNode>(m_dragStart.x, m_dragStart.y, 0, 0);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(ellipse)));
    } else if (m_tool == ToolType::Brush) {
        m_activeStroke.clear();
        m_activeStroke.push_back({m_dragStart, 1.0f, 0.0f});
    } else if (m_tool == ToolType::Image) {
        // Placeholder for image placement logic
        std::vector<uint8_t> dummyData(100 * 100 * 4, 255);
        auto image = std::make_unique<ImageNode>(dummyData, m_dragStart.x, m_dragStart.y, 100, 100);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(image)));
    } else if (m_tool == ToolType::Text) {
        auto text = std::make_unique<TextNode>("New Text", m_dragStart.x, m_dragStart.y);
        text->setFontSize(m_fontSize);
        // text->setFontFamily(m_fontFamily);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(text)));
    }
}

void WorkspaceStage::handleMouseMove(const Point2D& screenPos) {
    m_cursorCanvasPos = screenToCanvas(screenPos);
    if (!m_isDragging) return;

    if (m_tool == ToolType::Marquee) {
        updateMarquee(m_cursorCanvasPos);
    } else if (m_tool == ToolType::Brush) {
        m_activeStroke.push_back({m_cursorCanvasPos, 1.0f, 0.0f}); // Simplified pressure/velocity
    }
}

void WorkspaceStage::handleMouseUp() {
    if (m_tool == ToolType::Marquee) {
        performSelection(m_marqueeRect);
    } else if (m_tool == ToolType::Brush) {
        if (m_activeStroke.size() >= 2) {
            // Bake brush stroke into a PathNode (as requested by TASK 4)
            std::vector<BezierAnchor> anchors;
            for (const auto& pt : m_activeStroke) {
                anchors.emplace_back(pt.position, pt.position, pt.position);
            }
            auto path = std::make_unique<PathNode>(anchors);
            path->setStrokeWidth(m_brushSize);
            // In a real implementation, we might use a specialized BrushNode
            executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(path)));
        }
        m_activeStroke.clear();
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
