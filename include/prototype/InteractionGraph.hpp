#pragma once

#include "sync/CRDTTypes.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace vectma {

enum class TriggerType {
    MouseClick,
    MouseHover,
    MouseLeave,
    KeyPress
};

enum class TransitionType {
    Instant,
    Dissolve,
    SlideIn,
    SlideOut
};

struct PrototypeLink {
    NodeId sourceNodeId;
    NodeId targetArtboardId;
    TriggerType trigger = TriggerType::MouseClick;
    TransitionType transition = TransitionType::Instant;
    float easingCoefficients[4] = {0.25f, 0.1f, 0.25f, 1.0f}; // Default ease
    float durationMs = 300.0f;
    int keyCode = 0; // For KeyPress trigger

    bool operator==(const PrototypeLink& other) const {
        return sourceNodeId == other.sourceNodeId &&
               targetArtboardId == other.targetArtboardId &&
               trigger == other.trigger;
    }
};

struct NodeIdHash {
    std::size_t operator()(const NodeId& id) const {
        return std::hash<uint64_t>{}(id.timestamp) ^
               (std::hash<uint32_t>{}(id.clientId) << 1) ^
               (std::hash<uint32_t>{}(id.counter) << 2);
    }
};

class InteractionGraph {
public:
    static InteractionGraph& getInstance() {
        static InteractionGraph instance;
        return instance;
    }

    void addLink(const PrototypeLink& link);
    void removeLink(NodeId sourceNodeId, NodeId targetArtboardId, TriggerType trigger);
    void removeAllLinksFrom(NodeId sourceNodeId);

    std::vector<PrototypeLink> getLinksBySource(NodeId sourceNodeId) const;
    const PrototypeLink* findLink(NodeId sourceNodeId, TriggerType trigger) const;

    void onNodeDeleted(NodeId nodeId);

    std::vector<PrototypeLink> getAllLinks() const;

private:
    InteractionGraph() = default;

    mutable std::mutex m_mutex;
    std::unordered_map<NodeId, std::vector<PrototypeLink>, NodeIdHash> m_links;
};

} // namespace vectma
