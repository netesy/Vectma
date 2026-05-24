#pragma once

#include <vector>
#include <string>
#include "core/GRect.hpp"
#include "core/CanvasNode.hpp"

namespace vectma {

struct AlignmentGuide {
    GPoint start;
    GPoint end;
    double gapValue = 0.0;
    bool isVertical = false;
    bool hasDistanceCallout = false;
};

class AlignmentEngine {
public:
    static std::vector<AlignmentGuide> CalculateGuides(const GRect& activeBounds, CanvasNode* sceneRoot, const std::vector<CanvasNode*>& ignoredNodes);

private:
    static constexpr double SNAPPING_THRESHOLD = 4.0;
};

} // namespace vectma
