#pragma once

#include "core/CanvasNode.hpp"
#include "core/spatial/Quadtree.hpp"
#include <memory>
#include <vector>
#include <map>

namespace vectma {

class SceneGraph : public CanvasNode {
public:
    SceneGraph();
    virtual ~SceneGraph();

    std::string getClassName() const override { return "SceneGraph"; }

    void addChild(std::unique_ptr<CanvasNode> child);
    void addChildRemote(std::unique_ptr<CanvasNode> child, LamportTimestamp ts);

    std::unique_ptr<CanvasNode> removeChild(CanvasNode* node);
    void removeChildRemote(NodeId id, LamportTimestamp ts);

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;

    void bringToFront(size_t index);
    void sendToBack(size_t index);
    void moveUp(size_t index);
    void moveDown(size_t index);

    void groupNodes(const std::vector<size_t>& indices);
    void ungroupNode(size_t index);

    void clear();
    void rebuildIndex();
    std::vector<CanvasNode*> queryVisible(const GRect& viewport) const;

    CanvasNode* findNodeById(NodeId id) const;

private:
    std::unique_ptr<Quadtree> m_spatialIndex;
    std::map<NodeId, LamportTimestamp> m_tombstones;
    std::map<NodeId, LamportTimestamp> m_additions;
};

} // namespace vectma
