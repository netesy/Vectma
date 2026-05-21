#include "core/modifiers/OffsetPathModifier.hpp"

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
    if (m_distance == 0.0f) return output;

    // TODO: Implement stroke offset math for parallel boundaries from Phase 12
    clearDirty();
    return output;
}

} // namespace vectma
