#pragma once

#include <cmath>
#include <xmmintrin.h>
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

    // Vectorized mapping of 4 points in parallel using SSE instructions
    static void map4(const GTransform& t, const GPoint points[4], GPoint results[4]) {
        __m128 px = _mm_setr_ps(static_cast<float>(points[0].x), static_cast<float>(points[1].x), static_cast<float>(points[2].x), static_cast<float>(points[3].x));
        __m128 py = _mm_setr_ps(static_cast<float>(points[0].y), static_cast<float>(points[1].y), static_cast<float>(points[2].y), static_cast<float>(points[3].y));

        __m128 ta = _mm_set1_ps(static_cast<float>(t.a));
        __m128 tb = _mm_set1_ps(static_cast<float>(t.b));
        __m128 tc = _mm_set1_ps(static_cast<float>(t.c));
        __m128 td = _mm_set1_ps(static_cast<float>(t.d));
        __m128 te = _mm_set1_ps(static_cast<float>(t.e));
        __m128 tf = _mm_set1_ps(static_cast<float>(t.f));

        // rx = ta * px + tc * py + te
        __m128 rx = _mm_add_ps(_mm_add_ps(_mm_mul_ps(ta, px), _mm_mul_ps(tc, py)), te);
        // ry = tb * px + td * py + tf
        __m128 ry = _mm_add_ps(_mm_add_ps(_mm_mul_ps(tb, px), _mm_mul_ps(td, py)), tf);

        float tempx[4];
        float tempy[4];
        _mm_storeu_ps(tempx, rx);
        _mm_storeu_ps(tempy, ry);

        for (int i = 0; i < 4; ++i) {
            results[i].x = tempx[i];
            results[i].y = tempy[i];
        }
    }
};

} // namespace vectma
