#include "core/SceneGraph.hpp"
#include "renderer/RenderPipeline.hpp"
#include <algorithm>

namespace vectma {

SceneGraph::SceneGraph() = default;

SceneGraph::~SceneGraph() = default;

void SceneGraph::render(RenderPipeline& pipeline) const {
    for (const auto& child : m_children) {
        if (child && child->isVisible()) {
            child->render(pipeline);
        }
    }
}

bool SceneGraph::containsPoint(const GPoint& point) const {
    for (const auto& child : m_children) {
        if (child->containsPoint(point)) return true;
    }
    return false;
}

GRect SceneGraph::computeBoundingBox() const {
    GRect bbox;
    for (const auto& child : m_children) {
        bbox = bbox.united(child->computeBoundingBox());
    }
    return bbox;
}

void SceneGraph::bringToFront(size_t index) {
    if (index >= m_children.size() - 1) return;
    auto node = std::move(m_children[index]);
    m_children.erase(m_children.begin() + index);
    m_children.push_back(std::move(node));
}

void SceneGraph::sendToBack(size_t index) {
    if (index == 0 || index >= m_children.size()) return;
    auto node = std::move(m_children[index]);
    m_children.erase(m_children.begin() + index);
    m_children.insert(m_children.begin(), std::move(node));
}

void SceneGraph::moveUp(size_t index) {
    if (index >= m_children.size() - 1) return;
    std::swap(m_children[index], m_children[index + 1]);
}

void SceneGraph::moveDown(size_t index) {
    if (index == 0 || index >= m_children.size()) return;
    std::swap(m_children[index], m_children[index - 1]);
}

void SceneGraph::groupNodes(const std::vector<size_t>& indices) {
    if (indices.empty()) return;

    std::vector<size_t> sortedIndices = indices;
    std::sort(sortedIndices.rbegin(), sortedIndices.rend());

    auto group = std::make_unique<SceneGraph>();
    size_t insertAt = sortedIndices.back();

    for (size_t idx : sortedIndices) {
        if (idx < m_children.size()) {
            group->addChild(std::move(m_children[idx]));
            m_children.erase(m_children.begin() + idx);
        }
    }

    std::reverse(group->m_children.begin(), group->m_children.end());
    m_children.insert(m_children.begin() + insertAt, std::move(group));
}

void SceneGraph::ungroupNode(size_t index) {
    if (index >= m_children.size()) return;

    auto* group = dynamic_cast<SceneGraph*>(m_children[index].get());
    if (!group) return;

    auto groupNode = std::move(m_children[index]);
    m_children.erase(m_children.begin() + index);

    size_t insertAt = index;
    for (auto& child : group->m_children) {
        child->setParent(this);
        m_children.insert(m_children.begin() + insertAt, std::move(child));
        insertAt++;
    }
}

void SceneGraph::clear() {
    m_children.clear();
}

} // namespace vectma
