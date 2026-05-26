#pragma once

#include "core/GPoint.hpp"
#include "core/GRect.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace vectma {
namespace geometry {

enum class HandleRelation {
    Symmetric,
    Asymmetric,
    Disconnected
};

struct AnchorPoint {
    Point2D position;
    Point2D handleInOffset;   // Offset from position
    Point2D handleOutOffset;  // Offset from position
    HandleRelation relation = HandleRelation::Symmetric;

    Point2D getHandleIn() const { return {position.x + handleInOffset.x, position.y + handleInOffset.y}; }
    Point2D getHandleOut() const { return {position.x + handleOutOffset.x, position.y + handleOutOffset.y}; }

    void setHandleIn(Point2D absolutePos) {
        handleInOffset = {absolutePos.x - position.x, absolutePos.y - position.y};
        if (relation == HandleRelation::Symmetric) {
            handleOutOffset = {-handleInOffset.x, -handleInOffset.y};
        } else if (relation == HandleRelation::Asymmetric) {
            double lenIn = std::sqrt(handleInOffset.x * handleInOffset.x + handleInOffset.y * handleInOffset.y);
            double lenOut = std::sqrt(handleOutOffset.x * handleOutOffset.x + handleOutOffset.y * handleOutOffset.y);
            if (lenIn > 1e-6) {
                double ratio = lenOut / lenIn;
                handleOutOffset = {-handleInOffset.x * ratio, -handleInOffset.y * ratio};
            }
        }
    }

    void setHandleOut(Point2D absolutePos) {
        handleOutOffset = {absolutePos.x - position.x, absolutePos.y - position.y};
        if (relation == HandleRelation::Symmetric) {
            handleInOffset = {-handleOutOffset.x, -handleOutOffset.y};
        } else if (relation == HandleRelation::Asymmetric) {
            double lenOut = std::sqrt(handleOutOffset.x * handleOutOffset.x + handleOutOffset.y * handleOutOffset.y);
            double lenIn = std::sqrt(handleInOffset.x * handleInOffset.x + handleInOffset.y * handleInOffset.y);
            if (lenOut > 1e-6) {
                double ratio = lenIn / lenOut;
                handleInOffset = {-handleOutOffset.x * ratio, -handleOutOffset.y * ratio};
            }
        }
    }
};

struct VectorEdge {
    size_t fromIdx;
    size_t toIdx;
};

struct PathTopology {
    std::vector<AnchorPoint> points;
    std::vector<VectorEdge> edges;
    bool isClosed = false;
};

class BezierEvaluator {
public:
    static Point2D cubic(Point2D p0, Point2D p1, Point2D p2, Point2D p3, double t) {
        double t1 = 1.0 - t;
        return {
            t1*t1*t1*p0.x + 3*t1*t1*t*p1.x + 3*t1*t*t*p2.x + t*t*t*p3.x,
            t1*t1*t1*p0.y + 3*t1*t1*t*p1.y + 3*t1*t*t*p2.y + t*t*t*p3.y
        };
    }

    static double distanceToCubic(Point2D p0, Point2D p1, Point2D p2, Point2D p3, Point2D target, double& outT) {
        double minDistSq = 1e18;
        constexpr int steps = 100;
        for (int i = 0; i <= steps; ++i) {
            double t = (double)i / steps;
            Point2D p = cubic(p0, p1, p2, p3, t);
            double dx = p.x - target.x;
            double dy = p.y - target.y;
            double d2 = dx*dx + dy*dy;
            if (d2 < minDistSq) {
                minDistSq = d2;
                outT = t;
            }
        }
        return std::sqrt(minDistSq);
    }

    static GRect cubicBounds(Point2D p0, Point2D p1, Point2D p2, Point2D p3) {
        double minX = std::min({p0.x, p1.x, p2.x, p3.x});
        double minY = std::min({p0.y, p1.y, p2.y, p3.y});
        double maxX = std::max({p0.x, p1.x, p2.x, p3.x});
        double maxY = std::max({p0.y, p1.y, p2.y, p3.y});
        return GRect(minX, minY, maxX - minX, maxY - minY);
    }

    static double evaluateEasing(const float coefficients[4], double t) {
        if (t <= 0.0) return 0.0;
        if (t >= 1.0) return 1.0;

        Point2D p1 = { (double)coefficients[0], (double)coefficients[1] };
        Point2D p2 = { (double)coefficients[2], (double)coefficients[3] };

        double u = t;
        for (int i = 0; i < 8; ++i) {
            double x = 3*std::pow(1-u, 2)*u*p1.x + 3*(1-u)*u*u*p2.x + u*u*u;
            double dx = 3*std::pow(1-u, 2)*p1.x + 6*(1-u)*u*(p2.x - p1.x) + 3*u*u*(1 - p2.x);
            if (std::abs(dx) < 1e-6) break;
            u -= (x - t) / dx;
        }
        double y = 3*std::pow(1-u, 2)*u*p1.y + 3*(1-u)*u*u*p2.y + u*u*u;
        return std::clamp(y, 0.0, 1.0);
    }
};

} // namespace geometry
} // namespace vectma
