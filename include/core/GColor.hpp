#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include "core/GPoint.hpp"

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

    static GColor FromHex(const std::string& hex) {
        if (hex.empty()) return Black();
        size_t start = (hex[0] == '#') ? 1 : 0;
        uint32_t val = std::stoul(hex.substr(start), nullptr, 16);
        if (hex.length() - start == 6) {
            return GColor((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
        } else if (hex.length() - start == 8) {
            return GColor((val >> 24) & 0xFF, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
        }
        return Black();
    }
};

struct GradientStop {
    float offset = 0.0f;
    uint32_t color = 0xFFFFFFFF; // RGBA8

    GradientStop() = default;
    GradientStop(float off, uint32_t col) : offset(off), color(col) {}
};

enum class FillType { Solid, LinearGradient, RadialGradient };

struct GradientConfig {
    std::vector<GradientStop> stops;
    Point2D startPoint;
    Point2D endPoint;

    GradientConfig() {
        stops = { {0.0f, 0xFFFFFFFF}, {1.0f, 0x000000FF} };
        startPoint = {0, 0};
        endPoint = {100, 100};
    }
};

enum class StrokeAlignment { Center, Inside, Outside };

} // namespace vectma
