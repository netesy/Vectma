#include "core/modifiers/CornerRoundingModifier.hpp"
#include <cmath>
#include <vector>

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
    auto output = std::make_unique<PathData>();
    if (m_radius <= 0.0f) {
        *output = input;
        clearDirty();
        return output;
    }

    for (const auto& contour : input.contours) {
        if (contour.anchors.size() < 3) {
            output->contours.push_back(contour);
            continue;
        }

        std::vector<BezierAnchor> rounded;
        const auto& anchors = contour.anchors;
        size_t count = anchors.size();

        for (size_t i = 0; i < count; ++i) {
            size_t prev = (i == 0) ? (contour.isClosed ? count - 1 : 0) : i - 1;
            size_t next = (i == count - 1) ? (contour.isClosed ? 0 : count - 1) : i + 1;

            if (prev == i || next == i) {
                rounded.push_back(anchors[i]);
                continue;
            }

            Point2D p1 = anchors[prev].position;
            Point2D p2 = anchors[i].position;
            Point2D p3 = anchors[next].position;

            double v1x = p1.x - p2.x;
            double v1y = p1.y - p2.y;
            double v2x = p3.x - p2.x;
            double v2y = p3.y - p2.y;

            double d1 = std::sqrt(v1x * v1x + v1y * v1y);
            double d2 = std::sqrt(v2x * v2x + v2y * v2y);

            if (d1 < 1e-6 || d2 < 1e-6) {
                rounded.push_back(anchors[i]);
                continue;
            }

            double actualRadius = std::min((double)m_radius, std::min(d1 * 0.5, d2 * 0.5));
            Point2D start(p2.x + (v1x / d1) * actualRadius, p2.y + (v1y / d1) * actualRadius);
            Point2D end(p2.x + (v2x / d2) * actualRadius, p2.y + (v2y / d2) * actualRadius);

            rounded.emplace_back(start, start, start);
            rounded.emplace_back(end, end, end);
        }
        output->contours.emplace_back(rounded, contour.isClosed);
    }

    clearDirty();
    return output;
}

} // namespace vectma
