#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/GeometryEngine.hpp"
#include "core/Commands.hpp"
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
    if (m_selection.size() < 2 || !m_scene) return;

    PathNode* p1 = dynamic_cast<PathNode*>(m_selection[0]);
    PathNode* p2 = dynamic_cast<PathNode*>(m_selection[1]);

    if (p1 && p2) {
        auto result = GeometryEngine::combinePaths(*p1, *p2, op);
        if (result) {
            // executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(result)));
            m_scene->addChild(std::move(result));
            clearSelection();
        }
    }
}

void WorkspaceStage::handleMouseDown(const GPoint& screenPos) {
    m_isDragging = true;
    m_dragStart = screenToCanvas(screenPos);
    m_marqueeRect = GRect(m_dragStart.x, m_dragStart.y, 0, 0);

    if (m_subSelectionMode && !m_selection.empty()) {
        PathNode* path = dynamic_cast<PathNode*>(m_selection[0]);
        if (path) {
            int hit = path->hitTestAnchors(m_dragStart, 10.0f);
            if (hit != -1) {
                m_activeAnchorIndex = hit >> 2;
                m_activeHandleId = hit & 3;
            } else {
                m_activeAnchorIndex = -1;
                m_activeHandleId = -1;
            }
        }
    }
}

void WorkspaceStage::handleMouseMove(const GPoint& screenPos) {
    if (!m_isDragging) return;

    GPoint canvasPos = screenToCanvas(screenPos);

    if (m_subSelectionMode && m_activeAnchorIndex != -1) {
        PathNode* path = dynamic_cast<PathNode*>(m_selection[0]);
        if (path) {
            auto anchors = path->getAnchors();
            BezierAnchor& anchor = anchors[m_activeAnchorIndex];

            double dx = canvasPos.x - m_dragStart.x;
            double dy = canvasPos.y - m_dragStart.y;

            if (m_activeHandleId == 0) { // Position
                anchor.position.x += dx;
                anchor.position.y += dy;
                anchor.handleIn.x += dx;
                anchor.handleIn.y += dy;
                anchor.handleOut.x += dx;
                anchor.handleOut.y += dy;
            } else if (m_activeHandleId == 1) { // Handle In
                anchor.handleIn.x += dx;
                anchor.handleIn.y += dy;

                if (anchor.type == AnchorType::Symmetric) {
                    double rx = anchor.position.x - anchor.handleIn.x;
                    double ry = anchor.position.y - anchor.handleIn.y;
                    anchor.handleOut.x = anchor.position.x + rx;
                    anchor.handleOut.y = anchor.position.y + ry;
                }
            } else if (m_activeHandleId == 2) { // Handle Out
                anchor.handleOut.x += dx;
                anchor.handleOut.y += dy;

                if (anchor.type == AnchorType::Symmetric) {
                    double rx = anchor.position.x - anchor.handleOut.x;
                    double ry = anchor.position.y - anchor.handleOut.y;
                    anchor.handleIn.x = anchor.position.x + rx;
                    anchor.handleIn.y = anchor.position.y + ry;
                }
            }

            path->setAnchors(anchors);
            m_dragStart = canvasPos;
            return;
        }
    }

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
            auto node = std::make_unique<RectNode>(m_marqueeRect.x, m_marqueeRect.y, m_marqueeRect.width, m_marqueeRect.height);
            executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(node)));
        }
    } else if (m_tool == ToolType::Ellipse) {
        if (m_marqueeRect.width > 0 && m_marqueeRect.height > 0) {
            double rx = m_marqueeRect.width / 2.0;
            double ry = m_marqueeRect.height / 2.0;
            auto node = std::make_unique<EllipseNode>(m_marqueeRect.x + rx, m_marqueeRect.y + ry, rx, ry);
            executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(node)));
        }
    } else if (m_tool == ToolType::Path) {
        if (m_marqueeRect.width > 0 && m_marqueeRect.height > 0) {
            std::vector<BezierAnchor> anchors;
            anchors.emplace_back(Point2D(m_marqueeRect.x, m_marqueeRect.y), Point2D(m_marqueeRect.x - 20, m_marqueeRect.y), Point2D(m_marqueeRect.x + 20, m_marqueeRect.y));
            anchors.emplace_back(Point2D(m_marqueeRect.x + m_marqueeRect.width, m_marqueeRect.y + m_marqueeRect.height), Point2D(m_marqueeRect.x + m_marqueeRect.width - 20, m_marqueeRect.y + m_marqueeRect.height), Point2D(m_marqueeRect.x + m_marqueeRect.width + 20, m_marqueeRect.y + m_marqueeRect.height));
            auto node = std::make_unique<PathNode>(anchors);
            executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(node)));
        }
    } else if (m_tool == ToolType::Text) {
        auto font = std::make_shared<FontAsset>(std::vector<unsigned char>{});
        auto node = std::make_unique<TextNode>("New Text", font);
        executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(node)));
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

    for (const auto& child : m_scene->getChildren()) {
        GRect bbox = child->computeBoundingBox();
        if (bbox.x >= rect.x && bbox.x + bbox.width <= rect.x + rect.width &&
            bbox.y >= rect.y && bbox.y + bbox.height <= rect.y + rect.height) {
            addToSelection(child.get());
        }
    }
}

} // namespace vectma
