#include "core/modifiers/DashGeneratorModifier.hpp"
#include "core/GeometryEngine.hpp"
#include <cmath>

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
    auto output = std::make_unique<PathData>();
    if (m_pattern.empty()) {
        *output = input;
        clearDirty();
        return output;
    }

    for (const auto& contour : input.contours) {
        auto points = GeometryEngine::flattenContour(contour);
        if (points.size() < 2) {
            output->contours.push_back(contour);
            continue;
        }

        double patternTotal = 0;
        for(float f : m_pattern) patternTotal += f;

        double currentOffset = std::fmod((double)m_offset, patternTotal);
        if (currentOffset < 0) currentOffset += patternTotal;

        double walked = 0;
        size_t patternIndex = 0;
        bool drawing = true;
        while (walked + m_pattern[patternIndex] < currentOffset) {
            walked += m_pattern[patternIndex];
            patternIndex = (patternIndex + 1) % m_pattern.size();
            drawing = !drawing;
        }
        double nextSwitchDist = walked + m_pattern[patternIndex] - currentOffset;

        std::vector<Point2D> dashPoints;
        if (drawing) dashPoints.push_back(points[0]);

        for (size_t i = 0; i < points.size() - 1; ++i) {
            Point2D pA = points[i];
            Point2D pB = points[i+1];
            double dx = pB.x - pA.x;
            double dy = pB.y - pA.y;
            double d = std::sqrt(dx * dx + dy * dy);

            double currentInSegment = 0;
            while (currentInSegment + nextSwitchDist <= d) {
                currentInSegment += nextSwitchDist;
                double t = currentInSegment / d;
                Point2D splitP(pA.x + dx * t, pA.y + dy * t);

                if (drawing) {
                    dashPoints.push_back(splitP);
                    output->contours.push_back(GeometryEngine::pointsToContour(dashPoints, false));
                    dashPoints.clear();
                } else {
                    dashPoints.push_back(splitP);
                }

                drawing = !drawing;
                patternIndex = (patternIndex + 1) % m_pattern.size();
                nextSwitchDist = m_pattern[patternIndex];
            }

            nextSwitchDist -= (d - currentInSegment);
            if (drawing) {
                dashPoints.push_back(pB);
            }
        }

        if (drawing && dashPoints.size() >= 2) {
            output->contours.push_back(GeometryEngine::pointsToContour(dashPoints, false));
        }
    }

    clearDirty();
    return output;
}

} // namespace vectma
