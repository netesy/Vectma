#include "core/spatial/Quadtree.hpp"
#include <algorithm>

namespace vectma {

Quadtree::Quadtree(const GRect& bounds, size_t capacity, int maxDepth)
    : m_bounds(bounds), m_capacity(capacity), m_maxDepth(maxDepth), m_depth(0) {}

void Quadtree::insert(CanvasNode* node) {
    // If not in bounds, don't insert
    GRect bbox = node->computeBoundingBox();
    if (std::max(bbox.x, m_bounds.x) >= std::min(bbox.x + bbox.width, m_bounds.x + m_bounds.width) ||
        std::max(bbox.y, m_bounds.y) >= std::min(bbox.y + bbox.height, m_bounds.y + m_bounds.height)) {
        // Technically it might be better to allow insertion in root if it's huge
    }

    if (m_nodes.size() < m_capacity || m_depth >= m_maxDepth) {
        m_nodes.push_back(node);
        return;
    }

    if (!m_isDivided) {
        subdivide();
    }

    // Try to insert into children
    bool inserted = false;
    for (auto& child : m_children) {
        GRect cb = child->m_bounds;
        if (std::max(bbox.x, cb.x) < std::min(bbox.x + bbox.width, cb.x + cb.width) &&
            std::max(bbox.y, cb.y) < std::min(bbox.y + bbox.height, cb.y + cb.height)) {
            child->insert(node);
            inserted = true;
            break; // Insert into first intersecting child for simplicity
        }
    }

    if (!inserted) {
        m_nodes.push_back(node);
    }
}

void Quadtree::remove(CanvasNode* node) {
    auto it = std::find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        m_nodes.erase(it);
    } else if (m_isDivided) {
        for (auto& child : m_children) {
            child->remove(node);
        }
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

    // Check nodes in this level
    for (auto node : m_nodes) {
        GRect bbox = node->computeBoundingBox();
        if (std::max(bbox.x, range.x) < std::min(bbox.x + bbox.width, range.x + range.width) &&
            std::max(bbox.y, range.y) < std::min(bbox.y + bbox.height, range.y + range.height)) {
             result.push_back(node);
        }
    }

    // Check children
    if (m_isDivided) {
        for (auto& child : m_children) {
            // Check if child bounds intersects range
            if (std::max(child->m_bounds.x, range.x) < std::min(child->m_bounds.x + child->m_bounds.width, range.x + range.width) &&
                std::max(child->m_bounds.y, range.y) < std::min(child->m_bounds.y + child->m_bounds.height, range.y + range.height)) {
                auto childResult = child->query(range);
                result.insert(result.end(), childResult.begin(), childResult.end());
            }
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
