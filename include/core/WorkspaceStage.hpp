#pragma once

#include "core/SceneGraph.hpp"
#include "core/GTransform.hpp"
#include "core/Geometry.hpp"
#include "core/HistoryManager.hpp"
#include <memory>
#include <vector>

namespace vectma {

enum class ToolType { Select, Marquee, Rect, Ellipse, Path, Text };

/**
 * @brief State coordinator for active tools and selections.
 * Replaces the legacy GEditor.
 */
class WorkspaceStage {
public:
    WorkspaceStage();
    ~WorkspaceStage();

    void setScene(std::shared_ptr<SceneGraph> scene);
    std::shared_ptr<SceneGraph> getScene() const;

    // Selection
    void addToSelection(CanvasNode* node);
    void clearSelection();
    const std::vector<CanvasNode*>& getSelection() const;

    // Tools
    void setTool(ToolType tool);
    ToolType getTool() const { return m_tool; }

    // Sub-Selection Mode
    void setSubSelectionMode(bool active) { m_subSelectionMode = active; }
    bool isSubSelectionMode() const { return m_subSelectionMode; }
    int getActiveAnchorIndex() const { return m_activeAnchorIndex; }
    int getActiveHandleId() const { return m_activeHandleId; }

    // Boolean Operations
    void applyBooleanOperation(BooleanOp op);

    // History
    void undo() { m_history.undo(); }
    void redo() { m_history.redo(); }
    bool canUndo() const { return m_history.canUndo(); }
    bool canRedo() const { return m_history.canRedo(); }
    void executeCommand(std::unique_ptr<Command> cmd) { m_history.executeCommand(std::move(cmd)); }

    // Viewport
    void setViewMatrix(const GTransform& matrix) { m_viewMatrix = matrix; }
    GTransform getViewMatrix() const { return m_viewMatrix; }
    Point2D screenToCanvas(const Point2D& screenPos) const;

    // Mouse Lifecycle
    void handleMouseDown(const Point2D& screenPos);
    void handleMouseMove(const Point2D& screenPos);
    void handleMouseUp();

    // Interaction State
    bool isDragging() const { return m_isDragging; }
    GRect getMarqueeRect() const { return m_marqueeRect; }

private:
    std::shared_ptr<SceneGraph> m_scene;
    std::vector<CanvasNode*> m_selection;
    HistoryManager m_history;

    ToolType m_tool = ToolType::Select;
    GTransform m_viewMatrix = GTransform::Identity();

    bool m_isDragging = false;
    Point2D m_dragStart;
    GRect m_marqueeRect;

    // Sub-selection state
    bool m_subSelectionMode = false;
    int m_activeAnchorIndex = -1;
    int m_activeHandleId = -1; // 0: pos, 1: handleIn, 2: handleOut

    void updateMarquee(const Point2D& currentCanvasPos);
    void performSelection(const GRect& rect);
};

} // namespace vectma
