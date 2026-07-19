#include "core/modifiers/CornerRoundingModifier.hpp"
#include "style/TokenRegistry.hpp"
#include "core/LinearAllocator.hpp"
#include <cmath>
#include <vector>

// Thread-local pre-allocated cache vectors to eliminate transient heap allocations completely
thread_local std::vector<vectma::BezierAnchor> tl_roundedCache;

namespace vectma {

CornerRoundingModifier::CornerRoundingModifier(float radius)
    : m_radius(radius) {}

float CornerRoundingModifier::getRadius() const {
    if (m_radiusTokenPath) {
        return (float)TokenRegistry::getInstance().resolveAs<double>(*m_radiusTokenPath);
    }
    return m_radius;
}

void CornerRoundingModifier::setRadius(float radius) {
    if (m_radius != radius || m_radiusTokenPath) {
        m_radius = radius;
        m_radiusTokenPath = std::nullopt;
        markDirty();
    }
}

void CornerRoundingModifier::setRadiusToken(const std::string& path) {
    m_radiusTokenPath = path;
    markDirty();
}

std::unique_ptr<PathData> CornerRoundingModifier::apply(const PathData& input) const {
    auto output = std::make_unique<PathData>();

    float radius = getRadius();
    if (radius <= 0.0f) {
        *output = input;
        clearDirty();
        return output;
    }

    for (const auto& contour : input.contours) {
        if (contour.anchors.size() < 3) {
            output->contours.push_back(contour);
            continue;
        }

        tl_roundedCache.clear();
        const auto& anchors = contour.anchors;
        size_t count = anchors.size();

        for (size_t i = 0; i < count; ++i) {
            size_t prev = (i == 0) ? (contour.isClosed ? count - 1 : 0) : i - 1;
            size_t next = (i == count - 1) ? (contour.isClosed ? 0 : count - 1) : i + 1;

            if (prev == i || next == i) {
                tl_roundedCache.push_back(anchors[i]);
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
                tl_roundedCache.push_back(anchors[i]);
                continue;
            }

            double actualRadius = std::min((double)radius, std::min(d1 * 0.5, d2 * 0.5));
            Point2D start(p2.x + (v1x / d1) * actualRadius, p2.y + (v1y / d1) * actualRadius);
            Point2D end(p2.x + (v2x / d2) * actualRadius, p2.y + (v2y / d2) * actualRadius);

            tl_roundedCache.emplace_back(start, start, start);
            tl_roundedCache.emplace_back(end, end, end);
        }
        output->contours.emplace_back(tl_roundedCache, contour.isClosed);
    }

    clearDirty();
    return output;
}

} // namespace vectma
