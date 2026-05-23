#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/CompoundShapeNode.hpp"
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

GPoint WorkspaceStage::screenToCanvas(const GPoint& screenPos) const {
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
#include "core/WorkspaceStage.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/PathNode.hpp"
#include "core/Commands.hpp"

namespace vectma {

void WorkspaceStage::flattenCompoundShape(CompoundShapeNode* compound) {
    if (!compound) return;

    // Ideally, we'd get the resolved PathData from the compound shape.
    // Since our implementation delegates to Skia for drawing,
    // for a real "flatten" we would need to capture that SkPath or have a generic resolver.

    // For now, we'll implement a placeholder that creates a new PathNode
    // from the children's combined anchors (simulating a flatten Union).
    std::vector<BezierAnchor> allAnchors;
    for (const auto& child : compound->getChildren()) {
        if (auto* path = dynamic_cast<const PathNode*>(child.get())) {
            const auto& anchors = path->getAnchors();
            allAnchors.insert(allAnchors.end(), anchors.begin(), anchors.end());
        }
    }

    auto flatPath = std::make_unique<PathNode>(allAnchors);
    flatPath->setFillColor(compound->getFillColor());

    if (compound->getParent()) {
        auto* parent = dynamic_cast<SceneGraph*>(compound->getParent());
        if (parent) {
            parent->removeChild(compound);
            executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(flatPath)));
        }
    }
    clearSelection();
}

}
#include "core/WorkspaceStage.hpp"
#include "core/MasterComponentNode.hpp"
#include "core/ComponentInstanceNode.hpp"
#include "core/SymbolRegistry.hpp"
#include "core/Commands.hpp"

namespace vectma {

void WorkspaceStage::createComponentFromSelection() {
    if (m_selection.empty()) return;

    auto master = std::make_unique<MasterComponentNode>("Component " + std::to_string(m_selection.size()));
    std::vector<CanvasNode*> nodesToMove = m_selection;

    for (auto* node : nodesToMove) {
        if (node->getParent()) {
            auto removed = node->getParent()->removeChild(node);
            if (removed) master->addChild(std::move(removed));
        }
    }

    std::string guid = "comp_" + std::to_string(LamportClock::getInstance().tick().timestamp);
    auto* masterPtr = master.get();
    SymbolRegistry::getInstance().registerSymbol(guid, std::move(master));

    // Create an instance to replace the original selection
    auto instance = std::make_unique<ComponentInstanceNode>(masterPtr);
    executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(instance)));

    clearSelection();
}

void WorkspaceStage::detachInstance(ComponentInstanceNode* instance) {
    if (!instance || !instance->getMaster()) return;

    auto* master = instance->getMaster();
    auto* parent = dynamic_cast<SceneGraph*>(instance->getParent());
    if (!parent) return;

    // Clone all children from master into parent
    for (const auto& child : master->getChildren()) {
        auto clone = child->clone();
        // Apply overrides if we wanted to be perfect here
        parent->addChild(std::move(clone));
    }

    parent->removeChild(instance);
    clearSelection();
}

}
#include "core/WorkspaceStage.hpp"
#include "core/ComponentInstanceNode.hpp"
#include "core/SymbolRegistry.hpp"
#include "core/Commands.hpp"

namespace vectma {

void WorkspaceStage::placeInstance(const std::string& symbolId) {
    auto* master = dynamic_cast<MasterComponentNode*>(SymbolRegistry::getInstance().getSymbolRoot(symbolId));
    if (!master) return;

    auto instance = std::make_unique<ComponentInstanceNode>(master);
    // Place at center of viewport or current cursor
    (void)getCursorCanvasPos();
    // Assuming instance has some position properties or we just wrap it in a transform
    // For now, we'll just add it to the scene.

    executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(instance)));
}

}
