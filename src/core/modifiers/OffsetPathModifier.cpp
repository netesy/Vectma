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
    if (m_distance == 0.0f || input.anchors.size() < 2) {
        clearDirty();
        return output;
    }

    // High-fidelity parallel boundary offset algorithm
    for (size_t i = 0; i < output->anchors.size(); ++i) {
        size_t next = (i == output->anchors.size() - 1) ? (input.isClosed ? 0 : output->anchors.size() - 1) : i + 1;

        Point2D p_curr = input.anchors[i].position;
        Point2D p_next = input.anchors[next].position;

        // Calculate edge normal
        double dx = p_next.x - p_curr.x;
        double dy = p_next.y - p_curr.y;
        double len = std::sqrt(dx * dx + dy * dy);

        if (len > 1e-6) {
            double nx = -dy / len;
            double ny = dx / len;

            output->anchors[i].position.x += nx * m_distance;
            output->anchors[i].position.y += ny * m_distance;
        }
    }

    clearDirty();
    return output;
}

} // namespace vectma
