#pragma once
#include "core/GPoint.hpp"
#include "core/GColor.hpp"
#include <vector>

namespace vectma {

struct BrushPoint {
    Point2D position;
    float pressure = 1.0f;
    float velocity = 0.0f;
};

struct BrushSettings {
    float size = 10.0f;
    GColor color;
    float bleeding = 0.1f;
};

} // namespace vectma
