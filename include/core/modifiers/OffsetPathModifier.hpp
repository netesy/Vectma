#pragma once

#include "core/modifiers/Modifier.hpp"

namespace vectma {

class OffsetPathModifier : public Modifier {
public:
    OffsetPathModifier(float distance = 0.0f);

    std::string getModifierName() const override { return "OffsetPath"; }
    std::unique_ptr<PathData> apply(const PathData& input) const override;

    float getDistance() const { return m_distance; }
    void setDistance(float distance);

private:
    float m_distance;
};

} // namespace vectma
