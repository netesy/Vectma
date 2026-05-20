#pragma once

namespace vectma {

struct GPoint {
    double x = 0.0;
    double y = 0.0;

    GPoint() = default;
    GPoint(double x, double y) : x(x), y(y) {}
};

using Point2D = GPoint;

} // namespace vectma
