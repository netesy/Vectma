#pragma once

namespace vectma {

struct GPoint {
    double x = 0.0;
    double y = 0.0;

    GPoint() = default;
    GPoint(double x, double y) : x(x), y(y) {}
};

using Point2D = GPoint;

enum class AnchorType { Corner, Symmetric, Disconnected };

struct BezierAnchor {
    Point2D position;
    Point2D handleIn;
    Point2D handleOut;
    AnchorType type = AnchorType::Symmetric;

    BezierAnchor() = default;
    BezierAnchor(Point2D pos, Point2D hIn, Point2D hOut, AnchorType t = AnchorType::Symmetric)
        : position(pos), handleIn(hIn), handleOut(hOut), type(t) {}
};

} // namespace vectma
