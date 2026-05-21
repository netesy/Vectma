#include "core/modifiers/CornerRoundingModifier.hpp"

namespace vectma {

CornerRoundingModifier::CornerRoundingModifier(float radius)
    : m_radius(radius) {}

void CornerRoundingModifier::setRadius(float radius) {
    if (m_radius != radius) {
        m_radius = radius;
        markDirty();
    }
}

std::unique_ptr<PathData> CornerRoundingModifier::apply(const PathData& input) const {
    auto output = std::make_unique<PathData>(input);
    if (m_radius <= 0.0f) return output;

    // TODO: Implement analytical arc path computation logic from Phase 12
    // For now, it's a pass-through that marks it as processed
    clearDirty();
    return output;
}

} // namespace vectma
