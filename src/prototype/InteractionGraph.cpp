#include "prototype/InteractionGraph.hpp"
#include <algorithm>

namespace vectma {

void InteractionGraph::addLink(const PrototypeLink& link) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto& sourceLinks = m_links[link.sourceNodeId];

    auto it = std::find_if(sourceLinks.begin(), sourceLinks.end(), [&](const PrototypeLink& existing) {
        return existing.trigger == link.trigger;
    });

    if (it != sourceLinks.end()) {
        *it = link;
    } else {
        sourceLinks.push_back(link);
    }
}

void InteractionGraph::removeLink(NodeId sourceNodeId, NodeId targetArtboardId, TriggerType trigger) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_links.find(sourceNodeId);
    if (it != m_links.end()) {
        auto& sourceLinks = it->second;
        sourceLinks.erase(std::remove_if(sourceLinks.begin(), sourceLinks.end(), [&](const PrototypeLink& link) {
            return link.targetArtboardId == targetArtboardId && link.trigger == trigger;
        }), sourceLinks.end());

        if (sourceLinks.empty()) {
            m_links.erase(it);
        }
    }
}

void InteractionGraph::removeAllLinksFrom(NodeId sourceNodeId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_links.erase(sourceNodeId);
}

std::vector<PrototypeLink> InteractionGraph::getLinksBySource(NodeId sourceNodeId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_links.find(sourceNodeId);
    if (it != m_links.end()) {
        return it->second;
    }
    return {};
}

const PrototypeLink* InteractionGraph::findLink(NodeId sourceNodeId, TriggerType trigger) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_links.find(sourceNodeId);
    if (it != m_links.end()) {
        for (const auto& link : it->second) {
            if (link.trigger == trigger) return &link;
        }
    }
    return nullptr;
}

void InteractionGraph::onNodeDeleted(NodeId nodeId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_links.erase(nodeId);
    for (auto it = m_links.begin(); it != m_links.end(); ) {
        auto& sourceLinks = it->second;
        sourceLinks.erase(std::remove_if(sourceLinks.begin(), sourceLinks.end(), [&](const PrototypeLink& link) {
            return link.targetArtboardId == nodeId;
        }), sourceLinks.end());

        if (sourceLinks.empty()) {
            it = m_links.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<PrototypeLink> InteractionGraph::getAllLinks() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<PrototypeLink> all;
    for (const auto& [id, links] : m_links) {
        all.insert(all.end(), links.begin(), links.end());
    }
    return all;
}

} // namespace vectma
