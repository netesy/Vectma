#pragma once

#include "core/CanvasNode.hpp"
#include <memory>
#include <vector>

namespace vectma {

/**
 * @brief Composite structural layer tree.
 * Replaces the legacy GScene.
 */
class SceneGraph : public CanvasNode {
public:
    SceneGraph();
    virtual ~SceneGraph();

    std::string getClassName() const override { return "SceneGraph"; }

    // CanvasNode implementation
    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    // Z-Order Stacking Controls
    void bringToFront(size_t index);
    void sendToBack(size_t index);
    void moveUp(size_t index);
    void moveDown(size_t index);

    // Grouping / Ungrouping
    void groupNodes(const std::vector<size_t>& indices);
    void ungroupNode(size_t index);

    // Root-level management
    void clear();

private:
    // Root node is the SceneGraph itself
};

} // namespace vectma
