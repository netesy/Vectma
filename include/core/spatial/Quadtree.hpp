#pragma once

#include "core/CanvasNode.hpp"
#include <vector>
#include <memory>

namespace vectma {

struct FlatQuadNode {
    GRect bounds;
    std::vector<CanvasNode*> nodes;
    int childStartIndex = -1; // -1 if leaf, otherwise index of the first of 4 children
    bool isDivided = false;
};

class Quadtree {
public:
    Quadtree(const GRect& bounds, size_t capacity = 10, int maxDepth = 5);

    void insert(CanvasNode* node);
    void remove(CanvasNode* node);
    void update(CanvasNode* node);
    void clear();

    std::vector<CanvasNode*> query(const GRect& range) const;

private:
    GRect m_bounds;
    size_t m_capacity;
    int m_maxDepth;

    // High-performance DOD contiguous flat node pool
    mutable std::vector<FlatQuadNode> m_treeNodes;

    void insertRecursive(size_t nodeIndex, CanvasNode* node, int depth);
    void removeRecursive(size_t nodeIndex, CanvasNode* node);
    void queryRecursive(size_t nodeIndex, const GRect& range, std::vector<CanvasNode*>& results) const;
    void subdivide(size_t nodeIndex);
};

} // namespace vectma
