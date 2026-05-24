#pragma once

#include "core/modifiers/Modifier.hpp"
#include <string>
#include <optional>

namespace vectma {

class CornerRoundingModifier : public Modifier {
public:
    CornerRoundingModifier(float radius = 0.0f);

    std::string getModifierName() const override { return "CornerRounding"; }
    std::unique_ptr<PathData> apply(const PathData& input) const override;

    float getRadius() const;
    void setRadius(float radius);
    void setRadiusToken(const std::string& path);

private:
    float m_radius;
    std::optional<std::string> m_radiusTokenPath;
};

} // namespace vectma
