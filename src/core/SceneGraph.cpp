#include "core/SceneGraph.hpp"
#include "renderer/RenderPipeline.hpp"
#include <algorithm>

namespace vectma {

SceneGraph::SceneGraph() {
    m_spatialIndex = std::make_unique<Quadtree>(GRect(-10000, -10000, 20000, 20000));
}

SceneGraph::~SceneGraph() = default;

void SceneGraph::addChild(std::unique_ptr<CanvasNode> child) {
    CanvasNode::addChild(std::move(child));
    if (m_spatialIndex) m_spatialIndex->insert(m_children.back().get());
}

std::unique_ptr<CanvasNode> SceneGraph::removeChild(CanvasNode* node) {
    auto it = std::find_if(m_children.begin(), m_children.end(), [node](const auto& p) { return p.get() == node; });
    if (it != m_children.end()) {
        auto removed = std::move(*it);
        m_children.erase(it);
        if (m_spatialIndex) m_spatialIndex->remove(node);
        return removed;
    }
    return nullptr;
}

void SceneGraph::render(RenderPipeline& pipeline) const {
    GRect viewport(-10000, -10000, 20000, 20000);
    auto visibleNodes = m_spatialIndex->query(viewport);

    for (auto node : visibleNodes) {
        if (node && node->isVisible()) {
            node->render(pipeline);
        }
    }
}

bool SceneGraph::containsPoint(const GPoint& point) const {
    auto hits = m_spatialIndex->query(GRect(point.x - 1, point.y - 1, 2, 2));
    for (auto node : hits) {
        if (node->containsPoint(point)) return true;
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
    rebuildIndex();
}

void SceneGraph::sendToBack(size_t index) {
    if (index == 0 || index >= m_children.size()) return;
    auto node = std::move(m_children[index]);
    m_children.erase(m_children.begin() + index);
    m_children.insert(m_children.begin(), std::move(node));
    rebuildIndex();
}

void SceneGraph::moveUp(size_t index) {
    if (index >= m_children.size() - 1) return;
    std::swap(m_children[index], m_children[index + 1]);
    rebuildIndex();
}

void SceneGraph::moveDown(size_t index) {
    if (index == 0 || index >= m_children.size()) return;
    std::swap(m_children[index], m_children[index - 1]);
    rebuildIndex();
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
    rebuildIndex();
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
    rebuildIndex();
}

void SceneGraph::clear() {
    m_children.clear();
    m_spatialIndex->clear();
}

void SceneGraph::rebuildIndex() {
    m_spatialIndex->clear();
    for (const auto& child : m_children) {
        m_spatialIndex->insert(child.get());
    }
}

std::vector<CanvasNode*> SceneGraph::queryVisible(const GRect& viewport) const {
    return m_spatialIndex->query(viewport);
}

std::string SceneGraph::toSVG() const {
    std::string svg = "<svg xmlns=\"http://www.w3.org/2000/svg\">\n";
    svg += "<defs>\n";
    svg += "</defs>\n";
    for (const auto& child : m_children) {
        if (child) {
            svg += "  " + child->toSVG() + "\n";
        }
    }
    svg += "</svg>";
    return svg;
}

} // namespace vectma
