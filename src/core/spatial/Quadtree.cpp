#include "core/spatial/Quadtree.hpp"
#include <algorithm>

namespace vectma {

Quadtree::Quadtree(const GRect& bounds, size_t capacity, int maxDepth)
    : m_bounds(bounds), m_capacity(capacity), m_maxDepth(maxDepth) {
    clear();
}

void Quadtree::clear() {
    m_treeNodes.clear();
    m_treeNodes.push_back({m_bounds, {}, -1, false});
}

void Quadtree::insert(CanvasNode* node) {
    insertRecursive(0, node, 0);
}

void Quadtree::remove(CanvasNode* node) {
    removeRecursive(0, node);
}

void Quadtree::update(CanvasNode* node) {
    remove(node);
    insert(node);
}

std::vector<CanvasNode*> Quadtree::query(const GRect& range) const {
    std::vector<CanvasNode*> results;
    queryRecursive(0, range, results);
    return results;
}

void Quadtree::insertRecursive(size_t nodeIndex, CanvasNode* node, int depth) {
    if (!node || nodeIndex >= m_treeNodes.size()) return;

    GRect bbox = node->computeBoundingBox();
    const auto& currentBounds = m_treeNodes[nodeIndex].bounds;

    bool intersects = std::max(bbox.x, currentBounds.x) < std::min(bbox.x + bbox.width, currentBounds.x + currentBounds.width) &&
                      std::max(bbox.y, currentBounds.y) < std::min(bbox.y + bbox.height, currentBounds.y + currentBounds.height);

    if (!intersects && depth != 0) return; // Root takes everything that falls out for safety

    // If we have capacity or are at max depth, insert here
    if (m_treeNodes[nodeIndex].nodes.size() < m_capacity || depth >= m_maxDepth) {
        m_treeNodes[nodeIndex].nodes.push_back(node);
        return;
    }

    if (!m_treeNodes[nodeIndex].isDivided) {
        subdivide(nodeIndex);
    }

    bool insertedIntoChild = false;
    int childStart = m_treeNodes[nodeIndex].childStartIndex;
    for (int i = 0; i < 4; ++i) {
        size_t childIdx = childStart + i;
        if (childIdx < m_treeNodes.size()) {
            const auto& cb = m_treeNodes[childIdx].bounds;
            if (std::max(bbox.x, cb.x) < std::min(bbox.x + bbox.width, cb.x + cb.width) &&
                std::max(bbox.y, cb.y) < std::min(bbox.y + bbox.height, cb.y + cb.height)) {
                insertRecursive(childIdx, node, depth + 1);
                insertedIntoChild = true;
                break;
            }
        }
    }

    if (!insertedIntoChild) {
        m_treeNodes[nodeIndex].nodes.push_back(node);
    }
}

void Quadtree::removeRecursive(size_t nodeIndex, CanvasNode* node) {
    if (nodeIndex >= m_treeNodes.size()) return;

    auto& current = m_treeNodes[nodeIndex];
    auto it = std::find(current.nodes.begin(), current.nodes.end(), node);
    if (it != current.nodes.end()) {
        current.nodes.erase(it);
    } else if (current.isDivided) {
        int childStart = current.childStartIndex;
        for (int i = 0; i < 4; ++i) {
            removeRecursive(childStart + i, node);
        }
    }
}

void Quadtree::queryRecursive(size_t nodeIndex, const GRect& range, std::vector<CanvasNode*>& results) const {
    if (nodeIndex >= m_treeNodes.size()) return;

    const auto& current = m_treeNodes[nodeIndex];
    for (auto node : current.nodes) {
        GRect bbox = node->computeBoundingBox();
        if (std::max(bbox.x, range.x) < std::min(bbox.x + bbox.width, range.x + range.width) &&
            std::max(bbox.y, range.y) < std::min(bbox.y + bbox.height, range.y + range.height)) {
             results.push_back(node);
        }
    }

    if (current.isDivided) {
        int childStart = current.childStartIndex;
        GRect childRects[4];
        bool activeChildren[4] = {false, false, false, false};

        for (int i = 0; i < 4; ++i) {
            size_t childIdx = childStart + i;
            if (childIdx < m_treeNodes.size()) {
                childRects[i] = m_treeNodes[childIdx].bounds;
                activeChildren[i] = true;
            } else {
                childRects[i] = GRect(0, 0, 0, 0);
            }
        }

        bool intersectResults[4] = {false, false, false, false};
        GRect::intersect4(childRects, range, intersectResults);

        for (int i = 0; i < 4; ++i) {
            if (activeChildren[i] && intersectResults[i]) {
                queryRecursive(childStart + i, range, results);
            }
        }
    }
}

void Quadtree::subdivide(size_t nodeIndex) {
    // Avoid holding reference to vector element since push_back can reallocate vector
    GRect bounds = m_treeNodes[nodeIndex].bounds;
    int childStartIndex = static_cast<int>(m_treeNodes.size());

    double hw = bounds.width / 2.0;
    double hh = bounds.height / 2.0;

    m_treeNodes.push_back({GRect(bounds.x, bounds.y, hw, hh), {}, -1, false});
    m_treeNodes.push_back({GRect(bounds.x + hw, bounds.y, hw, hh), {}, -1, false});
    m_treeNodes.push_back({GRect(bounds.x, bounds.y + hh, hw, hh), {}, -1, false});
    m_treeNodes.push_back({GRect(bounds.x + hw, bounds.y + hh, hw, hh), {}, -1, false});

    // Write back index and flag safely
    m_treeNodes[nodeIndex].childStartIndex = childStartIndex;
    m_treeNodes[nodeIndex].isDivided = true;
}

} // namespace vectma
