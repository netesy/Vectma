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
struct LayoutSoACache {
    std::vector<NodeId> nodeIds;
    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> w;
    std::vector<double> h;
    std::vector<GColor> colors;
    std::vector<bool> visibility;
    std::vector<GRect> bounds;

    void clear() {
        nodeIds.clear();
        x.clear();
        y.clear();
        w.clear();
        h.clear();
        colors.clear();
        visibility.clear();
        bounds.clear();
    }

    void push(NodeId id, double nx, double ny, double nw, double nh, const GColor& col, bool vis, const GRect& bbox) {
        nodeIds.push_back(id);
        x.push_back(nx);
        y.push_back(ny);
        w.push_back(nw);
        h.push_back(nh);
        colors.push_back(col);
        visibility.push_back(vis);
        bounds.push_back(bbox);
    }
};

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
    std::unique_ptr<CanvasNode> clone() const override;

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

    // High-performance DOD cache controls
    const LayoutSoACache& getLayoutSoACache() const { return m_soaCache; }
    void updateSoACache() const;

private:
    std::unique_ptr<Quadtree> m_spatialIndex;
    mutable LayoutSoACache m_soaCache;

    void populateSoACacheRecursive(const CanvasNode* node) const;
};

} // namespace vectma

namespace vectma {
    // Phase 26: Style Cascade
    inline void MarkSceneDirty(CanvasNode* root) {
        if (!root) return;
        root->markLayoutDirty();
        for (const auto& child : root->getChildren()) {
            MarkSceneDirty(child.get());
        }
    }
}
