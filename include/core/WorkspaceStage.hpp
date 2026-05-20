#pragma once

#include "core/SceneGraph.hpp"
#include "core/GTransform.hpp"
#include <memory>
#include <vector>

namespace vectma {

enum class ToolType { Select, Marquee, Rect, Ellipse, Path };

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

    ToolType m_tool = ToolType::Select;
    GTransform m_viewMatrix = GTransform::Identity();

    bool m_isDragging = false;
    Point2D m_dragStart;
    GRect m_marqueeRect;

    void updateMarquee(const Point2D& currentCanvasPos);
    void performSelection(const GRect& rect);
};

} // namespace vectma
