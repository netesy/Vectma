#pragma once

#include <cmath>
#include "core/GPoint.hpp"

namespace vectma {

/**
 * @brief Affine transform matrix (3x3).
 * Represents:
 * [ a c e ]
 * [ b d f ]
 * [ 0 0 1 ]
 */
struct GTransform {
    double a = 1.0;
    double b = 0.0;
    double c = 0.0;
    double d = 1.0;
    double e = 0.0;
    double f = 0.0;

    GTransform() = default;
    GTransform(double a, double b, double c, double d, double e, double f)
        : a(a), b(b), c(c), d(d), e(e), f(f) {}

    static GTransform Identity() {
        return GTransform(1, 0, 0, 1, 0, 0);
    }

    static GTransform Translation(double tx, double ty) {
        return GTransform(1, 0, 0, 1, tx, ty);
    }

    GTransform multiply(const GTransform& other) const {
        return GTransform(
            a * other.a + c * other.b,
            b * other.a + d * other.b,
            a * other.c + c * other.d,
            b * other.c + d * other.d,
            a * other.e + c * other.f + e,
            b * other.e + d * other.f + f
        );
    }

    double det() const {
        return a * d - b * c;
    }

    GTransform inverse() const {
        double d_val = det();
        if (std::abs(d_val) < 1e-9) return Identity();
        double inv_det = 1.0 / d_val;
        return GTransform(
            d * inv_det,
            -b * inv_det,
            -c * inv_det,
            a * inv_det,
            (c * f - d * e) * inv_det,
            (b * e - a * f) * inv_det
        );
    }

    GPoint map(const GPoint& p) const {
        return GPoint(
            a * p.x + c * p.y + e,
            b * p.x + d * p.y + f
        );
    }
};

} // namespace vectma
