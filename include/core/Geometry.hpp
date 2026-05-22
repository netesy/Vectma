#pragma once

namespace vectma {

enum class BooleanOp {
    Union,
    Subtract,
    Intersect,
    Exclude
};

} // namespace vectma

enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Overlay,
    Darken,
    Lighten,
    ColorDodge,
    ColorBurn,
    HardLight,
    SoftLight,
    Difference,
    Exclusion
};
