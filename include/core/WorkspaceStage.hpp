#pragma once

#include "core/SceneGraph.hpp"
#include "core/GTransform.hpp"
#include "core/Geometry.hpp"
#include "core/HistoryManager.hpp"
#include "core/snap/SnappingEngine.hpp"
#include "core/BrushTypes.hpp"
#include "core/WaypointManager.hpp"
#include "geometry/BezierMath.hpp"
#include "ui/SelectionManager.hpp"
#include "core/AlignmentEngine.hpp"
#include "prototype/InteractionGraph.hpp"
#include <memory>
#include <vector>
#include <optional>
#include <string>

namespace vectma {

class PathNode;

enum class ToolType { Select, Marquee, Rect, Ellipse, Path, Text, Image, Brush, Pen, Artboard, Interaction };
enum class CanvasEditingMode { Normal, PathEdit };

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
    SelectionManager& getSelectionManager() { return m_selectionManager; }

    // Tools & Modes
    void setTool(ToolType tool);
    ToolType getTool() const { return m_tool; }

    void setEditingMode(CanvasEditingMode mode) { m_editingMode = mode; }
    CanvasEditingMode getEditingMode() const { return m_editingMode; }

    // UI State / Tool Parameters
    float getBrushSize() const { return m_brushSize; }
    void setBrushSize(float size) { m_brushSize = size; }
    float getBrushBleeding() const { return m_brushBleeding; }
    void setBrushBleeding(float bleeding) { m_brushBleeding = bleeding; }

    std::string getFontFamily() const { return m_fontFamily; }
    void setFontFamily(const std::string& family) { m_fontFamily = family; }
    float getFontSize() const { return m_fontSize; }
    void setFontSize(float size) { m_fontSize = size; }
    float getLineHeight() const { return m_lineHeight; }
    void setLineHeight(float lh) { m_lineHeight = lh; }

    // Sub-Selection State
    int getActiveAnchorIndex() const { return m_activeAnchorIndex; }
    int getActiveHandleId() const { return m_activeHandleId; }

    // Boolean Operations
    void applyBooleanOperation(BooleanOp op);
    void flattenCompoundShape(class CompoundShapeNode* compound);
    void createComponentFromSelection();
    void detachInstance(class ComponentInstanceNode* instance);
    void placeInstance(const std::string& symbolId);

    // History
    void undo() { m_history.undo(); }
    void redo() { m_history.redo(); }
    bool canUndo() const { return m_history.canUndo(); }
    bool canRedo() const { return m_history.canRedo(); }
    void executeCommand(std::unique_ptr<Command> cmd) { m_history.executeCommand(std::move(cmd)); }

    // Snapping & Guides
    std::optional<SnapResult> getActiveSnap() const { return m_activeSnap; }
    const std::vector<AlignmentGuide>& getAlignmentGuides() const { return m_alignmentGuides; }

    // Viewport
    void setViewMatrix(const GTransform& matrix) { m_viewMatrix = matrix; }
    GTransform getViewMatrix() const { return m_viewMatrix; }
    double getScale() const { return std::sqrt(m_viewMatrix.a * m_viewMatrix.d - m_viewMatrix.b * m_viewMatrix.c); }
    Point2D screenToCanvas(const Point2D& screenPos) const;

    // Pointer Interaction
    void handleMouseDown(const Point2D& screenPos, bool altPressed = false);
    void handleMouseMove(const Point2D& screenPos);
    void handleMouseUp();

    // Interaction State
    bool isDragging() const { return m_isDragging; }
    GRect getMarqueeRect() const { return m_marqueeRect; }
    Point2D getCursorCanvasPos() const { return m_cursorCanvasPos; }

    const std::vector<BrushPoint>& getActiveStroke() const { return m_activeStroke; }

    size_t getSceneNodeCount() const;

    // Waypoints & Animation
    WaypointManager& getWaypointManager() { return m_waypointManager; }
    void jumpToWaypoint(int slot);
    void tick(double dt);

    void setViewportSize(double w, double h) { m_viewportWidth = w; m_viewportHeight = h; }

    // Prototyping
    bool isDrawingWire() const { return m_isDrawingWire; }
    Point2D getWireTargetPos() const { return m_wireTargetPos; }

private:
    std::shared_ptr<SceneGraph> m_scene;
    SelectionManager m_selectionManager;
    HistoryManager m_history;

    ToolType m_tool = ToolType::Select;
    CanvasEditingMode m_editingMode = CanvasEditingMode::Normal;
    GTransform m_viewMatrix = GTransform::Identity();

    // Tool parameters
    float m_brushSize = 10.0f;
    float m_brushBleeding = 0.1f;
    std::string m_fontFamily = "Inter";
    float m_fontSize = 14.0f;
    float m_lineHeight = 1.2f;

    bool m_isDragging = false;
    Point2D m_dragStart;
    Point2D m_cursorCanvasPos;
    GRect m_marqueeRect;

    // Pen Tool / Path Edit state
    PathNode* m_activePathNode = nullptr;
    int m_activeAnchorIndex = -1;
    int m_activeHandleId = -1;

    // Brush state
    std::vector<BrushPoint> m_activeStroke;

    // Snapping state
    std::optional<SnapResult> m_activeSnap;
    std::vector<AlignmentGuide> m_alignmentGuides;

    // Waypoints & Easing
    WaypointManager m_waypointManager;
    bool m_isAnimating = false;
    double m_animationTime = 0.0;
    static constexpr double ANIMATION_DURATION = 0.3; // 300ms

    double m_startCenterX, m_startCenterY, m_startZoom;
    double m_targetCenterX, m_targetCenterY, m_targetZoom;
    double m_viewportWidth = 1920, m_viewportHeight = 1080;

    // Prototyping
    bool m_isDrawingWire = false;
    Point2D m_wireTargetPos;

    void updateMarquee(const Point2D& currentCanvasPos);
    void performSelection(const GRect& rect);
    size_t countNodesRecursive(const CanvasNode* node) const;

    void updateViewMatrixFromCenter(double cx, double cy, double zoom);
    Point2D getViewportCenterInCanvas() const;
};

} // namespace vectma
