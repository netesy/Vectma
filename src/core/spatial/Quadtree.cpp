#include "core/spatial/Quadtree.hpp"
#include <algorithm>

namespace vectma {

Quadtree::Quadtree(const GRect& bounds, size_t capacity, int maxDepth)
    : m_bounds(bounds), m_capacity(capacity), m_maxDepth(maxDepth), m_depth(0) {}

void Quadtree::insert(CanvasNode* node) {
    if (m_nodes.size() < m_capacity || m_depth >= m_maxDepth) {
        m_nodes.push_back(node);
        return;
    }

    if (!m_isDivided) {
        subdivide();
    }

    m_nodes.push_back(node);
}

void Quadtree::remove(CanvasNode* node) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        m_nodes.erase(it);
    }
}

void Quadtree::update(CanvasNode* node) {
    remove(node);
    insert(node);
}

void Quadtree::clear() {
    m_nodes.clear();
    for (auto& child : m_children) {
        if (child) child->clear();
        child.reset();
    }
    m_isDivided = false;
}

std::vector<CanvasNode*> Quadtree::query(const GRect& range) const {
    std::vector<CanvasNode*> result;
    for (auto node : m_nodes) {
        GRect bbox = node->computeBoundingBox();
        if (std::max(bbox.x, range.x) < std::min(bbox.x + bbox.width, range.x + range.width) &&
            std::max(bbox.y, range.y) < std::min(bbox.y + bbox.height, range.y + range.height)) {
             result.push_back(node);
        }
    }
    return result;
}

void Quadtree::subdivide() {
    m_isDivided = true;
    double hw = m_bounds.width / 2.0;
    double hh = m_bounds.height / 2.0;

    m_children[0] = std::make_unique<Quadtree>(GRect(m_bounds.x, m_bounds.y, hw, hh), m_capacity, m_maxDepth);
    m_children[1] = std::make_unique<Quadtree>(GRect(m_bounds.x + hw, m_bounds.y, hw, hh), m_capacity, m_maxDepth);
    m_children[2] = std::make_unique<Quadtree>(GRect(m_bounds.x, m_bounds.y + hh, hw, hh), m_capacity, m_maxDepth);
    m_children[3] = std::make_unique<Quadtree>(GRect(m_bounds.x + hw, m_bounds.y + hh, hw, hh), m_capacity, m_maxDepth);

    for (auto& child : m_children) child->m_depth = m_depth + 1;
}

} // namespace vectma
