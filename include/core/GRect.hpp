#pragma once

#include <algorithm>

namespace vectma {

struct GRect {
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;

    GRect() = default;
    GRect(double x, double y, double w, double h) : x(x), y(y), width(w), height(h) {}

    bool contains(double px, double py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }

    GRect united(const GRect& other) const {
        if (width <= 0 && height <= 0) return other;
        if (other.width <= 0 && other.height <= 0) return *this;

        double nx = std::min(x, other.x);
        double ny = std::min(y, other.y);
        double nw = std::max(x + width, other.x + other.width) - nx;
        double nh = std::max(y + height, other.y + other.height) - ny;
        return GRect(nx, ny, nw, nh);
    }
};

} // namespace vectma
