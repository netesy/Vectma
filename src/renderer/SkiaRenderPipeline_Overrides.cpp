#include "renderer/SkiaRenderPipeline.hpp"
#include "core/PropertyOverride.hpp"
#include <iostream>

namespace vectma {

#ifdef VECTMA_USE_SKIA

void SkiaRenderPipeline::pushOverrideContext(const OverrideMap* overrides) {
    m_overrideStack.push_back(overrides);
}

void SkiaRenderPipeline::popOverrideContext() {
    if (!m_overrideStack.empty()) {
        m_overrideStack.pop_back();
    }
}

template <typename T>
T ResolveProperty(const std::vector<const OverrideMap*>& stack, NodeId nodeId, const std::string& prop, T defaultValue) {
    // Traverse stack from top to bottom (most recent overrides first)
    std::string key = std::to_string(nodeId.timestamp) + "." + prop;
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        auto map = *it;
        auto found = map->find(key);
        if (found != map->end()) {
            try {
                return std::get<T>(found->second);
            } catch (...) {
                // Type mismatch, fallback
            }
        }
    }
    return defaultValue;
}

#endif

}
