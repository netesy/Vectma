#pragma once

#include <cstdint>

namespace vectma {

struct GColor {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;

    GColor() = default;
    GColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}

    static GColor Black() { return GColor(0, 0, 0); }
    static GColor White() { return GColor(255, 255, 255); }
    static GColor Transparent() { return GColor(0, 0, 0, 0); }
};

} // namespace vectma
