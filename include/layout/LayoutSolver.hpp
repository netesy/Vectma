#pragma once

#include <vector>
#include <memory>

namespace vectma {

class CanvasNode;

enum class LayoutDirection {
    Horizontal,
    Vertical
};

enum class SizingRule {
    Fixed,
    HugContents,
    FillContainer
};

enum class LayoutAlignment {
    Start,
    Center,
    End,
    SpaceBetween
};

struct Padding {
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float left = 0.0f;
};

struct LayoutProperties {
    bool enabled = false;
    LayoutDirection direction = LayoutDirection::Horizontal;
    Padding padding;
    float gap = 0.0f;
    LayoutAlignment mainAxisAlign = LayoutAlignment::Start;
    LayoutAlignment counterAxisAlign = LayoutAlignment::Start;
};

class LayoutSolver {
public:
    /**
     * @brief Resolves all auto-layout constraints for the given node tree.
     * Executes two passes: Measure (bottom-up) and Layout (top-down).
     */
    static void ResolveConstraints(CanvasNode* root);

private:
    static void measureNode(CanvasNode* node);
    static void layoutNode(CanvasNode* node, float availableWidth, float availableHeight);
};

} // namespace vectma
