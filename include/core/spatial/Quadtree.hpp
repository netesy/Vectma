#pragma once

#include "core/CanvasNode.hpp"
#include <vector>
#include <memory>

namespace vectma {

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
    int m_depth;

    std::vector<CanvasNode*> m_nodes;
    std::unique_ptr<Quadtree> m_children[4];
    bool m_isDivided = false;

    void subdivide();
    int getChildIndex(const GRect& bbox) const;
};

} // namespace vectma
