#pragma once

#include "core/modifiers/Modifier.hpp"

namespace vectma {

class CornerRoundingModifier : public Modifier {
public:
    CornerRoundingModifier(float radius = 0.0f);

    std::string getModifierName() const override { return "CornerRounding"; }
    std::unique_ptr<PathData> apply(const PathData& input) const override;

    float getRadius() const { return m_radius; }
    void setRadius(float radius);

private:
    float m_radius;
};

} // namespace vectma
