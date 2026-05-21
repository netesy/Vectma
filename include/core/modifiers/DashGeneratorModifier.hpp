#pragma once

#include "core/modifiers/Modifier.hpp"
#include <vector>

namespace vectma {

class DashGeneratorModifier : public Modifier {
public:
    DashGeneratorModifier(const std::vector<float>& pattern = {}, float offset = 0.0f);

    std::string getModifierName() const override { return "DashGenerator"; }
    std::unique_ptr<PathData> apply(const PathData& input) const override;

    const std::vector<float>& getPattern() const { return m_pattern; }
    void setPattern(const std::vector<float>& pattern);

    float getOffset() const { return m_offset; }
    void setOffset(float offset);

private:
    std::vector<float> m_pattern;
    float m_offset;
};

} // namespace vectma
