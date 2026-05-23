#pragma once

#include "core/CanvasNode.hpp"
#include "core/spatial/Quadtree.hpp"
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

    // Overrides
    void addChild(std::unique_ptr<CanvasNode> child);
    std::unique_ptr<CanvasNode> removeChild(CanvasNode* node);

    // CanvasNode implementation
    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;

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

    // Spatial Index
    void rebuildIndex();
    std::vector<CanvasNode*> queryVisible(const GRect& viewport) const;

private:
    std::unique_ptr<Quadtree> m_spatialIndex;
};

} // namespace vectma
