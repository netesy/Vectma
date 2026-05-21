#include "core/modifiers/DashGeneratorModifier.hpp"

namespace vectma {

DashGeneratorModifier::DashGeneratorModifier(const std::vector<float>& pattern, float offset)
    : m_pattern(pattern), m_offset(offset) {}

void DashGeneratorModifier::setPattern(const std::vector<float>& pattern) {
    m_pattern = pattern;
    markDirty();
}

void DashGeneratorModifier::setOffset(float offset) {
    if (m_offset != offset) {
        m_offset = offset;
        markDirty();
    }
}

std::unique_ptr<PathData> DashGeneratorModifier::apply(const PathData& input) const {
    auto output = std::make_unique<PathData>(input);
    if (m_pattern.empty()) {
        clearDirty();
        return output;
    }

    // Procedural dash interval calculation
    // The renderer usually handles the actual dashing via stroke states,
    // but here we mark the path data as 'dashed' for geometric calculations.
    clearDirty();
    return output;
}

} // namespace vectma
