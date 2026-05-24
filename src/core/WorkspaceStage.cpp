#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/ArtboardNode.hpp"
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
    m_activePathNode = nullptr;
}

const std::vector<CanvasNode*>& WorkspaceStage::getSelection() const {
    return m_selection;
}

void WorkspaceStage::setTool(ToolType tool) {
    m_tool = tool;
    m_isDragging = false;
    m_activeStroke.clear();
    if (tool == ToolType::Pen) {
        setEditingMode(CanvasEditingMode::PathEdit);
    } else {
        setEditingMode(CanvasEditingMode::Normal);
    }
}

Point2D WorkspaceStage::screenToCanvas(const Point2D& screenPos) const {
    return m_viewMatrix.inverse().map(screenPos);
}

void WorkspaceStage::applyBooleanOperation(BooleanOp op) {
    if (m_selection.size() < 2) return;

    BooleanOpType opType;
    switch(op) {
        case BooleanOp::Union:     opType = BooleanOpType::Union; break;
        case BooleanOp::Subtract:  opType = BooleanOpType::Subtract; break;
        case BooleanOp::Intersect: opType = BooleanOpType::Intersect; break;
        case BooleanOp::Exclude:   opType = BooleanOpType::Exclude; break;
        default:                   opType = BooleanOpType::Union; break;
    }

    auto compound = std::make_unique<CompoundShapeNode>(opType);
    std::vector<CanvasNode*> nodesToMove = m_selection;

    for (auto* node : nodesToMove) {
        if (node->getParent()) {
            auto* parent = dynamic_cast<SceneGraph*>(node->getParent());
            if (parent) {
                auto removed = parent->removeChild(node);
                if (removed) compound->addChild(std::move(removed));
            }
        }
    }

    auto* compoundPtr = compound.get();
    executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(compound)));
    clearSelection();
    addToSelection(compoundPtr);
}

void WorkspaceStage::handleMouseDown(const Point2D& screenPos, bool altPressed) {
    m_isDragging = true;
    m_dragStart = screenToCanvas(screenPos);
    m_cursorCanvasPos = m_dragStart;

    if (m_editingMode == CanvasEditingMode::PathEdit) {
        if (!m_activePathNode) {
            auto newNode = std::make_unique<PathNode>();
            m_activePathNode = newNode.get();
            executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(newNode)));
            addToSelection(m_activePathNode);
        }
        geometry::AnchorPoint newPoint;
        newPoint.position = m_dragStart;
        newPoint.handleInOffset = {0, 0};
        newPoint.handleOutOffset = {0, 0};
        newPoint.relation = altPressed ? geometry::HandleRelation::Disconnected : geometry::HandleRelation::Symmetric;
        m_activePathNode->addAnchor(newPoint);
        m_activeAnchorIndex = m_activePathNode->getTopology().points.size() - 1;
        m_activeHandleId = 2;
    } else if (m_tool == ToolType::Rect) {
        auto rect = std::make_unique<RectNode>(m_dragStart.x, m_dragStart.y, 0, 0);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(rect)));
    } else if (m_tool == ToolType::Brush) {
        m_activeStroke.clear();
        m_activeStroke.push_back({m_dragStart, 1.0f, 0.0f});
    } else if (m_tool == ToolType::Artboard) {
        auto ab = std::make_unique<ArtboardNode>("Artboard", GRect(m_dragStart.x, m_dragStart.y, 0, 0));
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(ab)));
    }
}

void WorkspaceStage::handleMouseMove(const Point2D& screenPos) {
    m_cursorCanvasPos = screenToCanvas(screenPos);
    if (!m_isDragging) return;

    if (m_editingMode == CanvasEditingMode::PathEdit && m_activePathNode) {
        auto topology = m_activePathNode->getTopology();
        if (m_activeAnchorIndex >= 0 && (size_t)m_activeAnchorIndex < topology.points.size()) {
            topology.points[m_activeAnchorIndex].setHandleOut(m_cursorCanvasPos);
            m_activePathNode->setTopology(topology);
        }
    } else if (m_tool == ToolType::Marquee) {
        updateMarquee(m_cursorCanvasPos);
    } else if (m_tool == ToolType::Brush) {
        m_activeStroke.push_back({m_cursorCanvasPos, 1.0f, 0.0f});
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
