#pragma once

#include <algorithm>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <xmmintrin.h>
#define VECTMA_SSE_SUPPORTED 1
#endif

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

    // High-performance vectorized bounds culling:
    // Performs parallel intersection tests for 4 GRect bounds against a target viewport query
    static void intersect4(const GRect rects[4], const GRect& query, bool results[4]) {
#if VECTMA_SSE_SUPPORTED
        // Load four x coordinates
        __m128 rx = _mm_setr_ps(static_cast<float>(rects[0].x), static_cast<float>(rects[1].x), static_cast<float>(rects[2].x), static_cast<float>(rects[3].x));
        __m128 ry = _mm_setr_ps(static_cast<float>(rects[0].y), static_cast<float>(rects[1].y), static_cast<float>(rects[2].y), static_cast<float>(rects[3].y));
        __m128 rw = _mm_setr_ps(static_cast<float>(rects[0].width), static_cast<float>(rects[1].width), static_cast<float>(rects[2].width), static_cast<float>(rects[3].width));
        __m128 rh = _mm_setr_ps(static_cast<float>(rects[0].height), static_cast<float>(rects[1].height), static_cast<float>(rects[2].height), static_cast<float>(rects[3].height));

        __m128 qx = _mm_set1_ps(static_cast<float>(query.x));
        __m128 qy = _mm_set1_ps(static_cast<float>(query.y));
        __m128 qw = _mm_set1_ps(static_cast<float>(query.width));
        __m128 qh = _mm_set1_ps(static_cast<float>(query.height));

        // x2 = x + width, y2 = y + height
        __m128 rx2 = _mm_add_ps(rx, rw);
        __m128 ry2 = _mm_add_ps(ry, rh);
        __m128 qx2 = _mm_add_ps(qx, qw);
        __m128 qy2 = _mm_add_ps(qy, qh);

        // Check intersection conditions:
        // std::max(rx, qx) < std::min(rx2, qx2)
        __m128 max_x = _mm_max_ps(rx, qx);
        __m128 min_x2 = _mm_min_ps(rx2, qx2);
        __m128 cmp_x = _mm_cmplt_ps(max_x, min_x2);

        // std::max(ry, qy) < std::min(ry2, qy2)
        __m128 max_y = _mm_max_ps(ry, qy);
        __m128 min_y2 = _mm_min_ps(ry2, qy2);
        __m128 cmp_y = _mm_cmplt_ps(max_y, min_y2);

        // both conditions must be true
        __m128 cmp_res = _mm_and_ps(cmp_x, cmp_y);

        // Extract results
        float temp[4];
        _mm_storeu_ps(temp, cmp_res);
        for (int i = 0; i < 4; ++i) {
            results[i] = (temp[i] != 0.0f);
        }
#else
        // Fully correct, auto-vectorizable scalar fallback path on non-x86 architectures
        for (int i = 0; i < 4; ++i) {
            const auto& r = rects[i];
            results[i] = std::max(r.x, query.x) < std::min(r.x + r.width, query.x + query.width) &&
                         std::max(r.y, query.y) < std::min(r.y + r.height, query.y + query.height);
        }
#endif
    }
};

} // namespace vectma
