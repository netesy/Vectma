#include "core/modifiers/OffsetPathModifier.hpp"
#include "core/GeometryEngine.hpp"
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
    auto output = std::make_unique<PathData>();
    if (m_distance == 0.0f) {
        *output = input;
        clearDirty();
        return output;
    }

    for (const auto& contour : input.contours) {
        if (contour.anchors.size() < 2) {
            output->contours.push_back(contour);
            continue;
        }

        // For high-fidelity offsetting, we flatten the path first
        auto points = GeometryEngine::flattenContour(contour);
        if (points.size() < 2) continue;

        std::vector<Point2D> offsetPoints;
        size_t n = points.size();

        for (size_t i = 0; i < n; ++i) {
            size_t prev = (i == 0) ? (contour.isClosed ? n - 1 : 0) : i - 1;
            size_t next = (i == n - 1) ? (contour.isClosed ? 0 : n - 1) : i + 1;

            Point2D p1 = points[prev];
            Point2D p2 = points[i];
            Point2D p3 = points[next];

            // Inward/Outward normal calculation
            auto getNormal = [](Point2D a, Point2D b) {
                double dx = b.x - a.x;
                double dy = b.y - a.y;
                double len = std::sqrt(dx * dx + dy * dy);
                if (len < 1e-6) return Point2D(0, 0);
                return Point2D(-dy / len, dx / len);
            };

            Point2D n1 = getNormal(p1, p2);
            Point2D n2 = getNormal(p2, p3);

            if (i == 0 && !contour.isClosed) {
                offsetPoints.emplace_back(p2.x + n2.x * m_distance, p2.y + n2.y * m_distance);
            } else if (i == n - 1 && !contour.isClosed) {
                offsetPoints.emplace_back(p2.x + n1.x * m_distance, p2.y + n1.y * m_distance);
            } else {
                // Average normal for join
                double nx = (n1.x + n2.x);
                double ny = (n1.y + n2.y);
                double nlen = std::sqrt(nx * nx + ny * ny);
                if (nlen > 1e-6) {
                    // Miter length factor
                    double cosAlpha = n1.x * n2.x + n1.y * n2.y;
                    double miterLimit = 4.0;
                    double factor = 2.0 / (1.0 + cosAlpha);
                    if (factor > miterLimit * miterLimit) factor = miterLimit * miterLimit; // Clamp miter

                    double miterScale = std::sqrt(factor);
                    offsetPoints.emplace_back(p2.x + (nx / nlen) * m_distance * miterScale,
                                            p2.y + (ny / nlen) * m_distance * miterScale);
                } else {
                    offsetPoints.emplace_back(p2.x + n1.x * m_distance, p2.y + n1.y * m_distance);
                }
            }
        }
        output->contours.push_back(GeometryEngine::pointsToContour(offsetPoints, contour.isClosed));
    }

    clearDirty();
    return output;
}

} // namespace vectma
