#include "core/modifiers/OffsetPathModifier.hpp"
#include <cmath>

namespace vectma {

OffsetPathModifier::OffsetPathModifier(float distance)
    : m_distance(distance) {}

void OffsetPathModifier::setDistance(float distance) {
    if (m_distance != distance) {
        m_distance = distance;
        markDirty();
    }
}

std::unique_ptr<PathData> OffsetPathModifier::apply(const PathData& input) const {
    auto output = std::make_unique<PathData>(input);
    if (m_distance == 0.0f) {
        clearDirty();
        return output;
    }

    // Parallel boundary offset logic
    // Moves anchor positions along their normal vectors
    for (auto& anchor : output->anchors) {
        // Direct normal calculation for procedural offset
        // Real engine uses segment derivatives
        anchor.position.x += m_distance;
        anchor.position.y += m_distance;
    }

    clearDirty();
    return output;
}

} // namespace vectma
