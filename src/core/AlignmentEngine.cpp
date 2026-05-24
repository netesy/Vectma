#include "core/AlignmentEngine.hpp"
#include <cmath>
#include <algorithm>
#include <functional>

namespace vectma {

std::vector<AlignmentGuide> AlignmentEngine::CalculateGuides(const GRect& activeBounds, CanvasNode* sceneRoot, const std::vector<CanvasNode*>& ignoredNodes) {
    std::vector<AlignmentGuide> guides;
    if (!sceneRoot) return guides;

    auto isIgnored = [&](CanvasNode* node) {
        return std::find(ignoredNodes.begin(), ignoredNodes.end(), node) != ignoredNodes.end();
    };

    double edgesX[] = { activeBounds.x, activeBounds.x + activeBounds.width / 2.0, activeBounds.x + activeBounds.width };
    double edgesY[] = { activeBounds.y, activeBounds.y + activeBounds.height / 2.0, activeBounds.y + activeBounds.height };

    std::function<void(CanvasNode*)> traverse = [&](CanvasNode* node) {
        if (node != sceneRoot && !isIgnored(node)) {
            GRect otherBounds = node->computeBoundingBox();
            double otherX[] = { otherBounds.x, otherBounds.x + otherBounds.width / 2.0, otherBounds.x + otherBounds.width };
            double otherY[] = { otherBounds.y, otherBounds.y + otherBounds.height / 2.0, otherBounds.y + otherBounds.height };

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    if (std::abs(edgesX[i] - otherX[j]) < SNAPPING_THRESHOLD) {
                        AlignmentGuide g;
                        g.isVertical = true;
                        g.start = { otherX[j], std::min(activeBounds.y, otherBounds.y) - 20 };
                        g.end = { otherX[j], std::max(activeBounds.y + activeBounds.height, otherBounds.y + otherBounds.height) + 20 };

                        // Distance callout
                        if (i == 2 && j == 0) { // Right edge of selection to left edge of other
                            g.hasDistanceCallout = true;
                            g.gapValue = std::abs(activeBounds.x + activeBounds.width - otherBounds.x);
                        } else if (i == 0 && j == 2) { // Left edge of selection to right edge of other
                            g.hasDistanceCallout = true;
                            g.gapValue = std::abs(otherBounds.x + otherBounds.width - activeBounds.x);
                        }

                        guides.push_back(g);
                    }
                    if (std::abs(edgesY[i] - otherY[j]) < SNAPPING_THRESHOLD) {
                        AlignmentGuide g;
                        g.isVertical = false;
                        g.start = { std::min(activeBounds.x, otherBounds.x) - 20, otherY[j] };
                        g.end = { std::max(activeBounds.x + activeBounds.width, otherBounds.x + otherBounds.width) + 20, otherY[j] };

                        if (i == 2 && j == 0) { // Bottom to Top
                            g.hasDistanceCallout = true;
                            g.gapValue = std::abs(activeBounds.y + activeBounds.height - otherBounds.y);
                        } else if (i == 0 && j == 2) { // Top to Bottom
                            g.hasDistanceCallout = true;
                            g.gapValue = std::abs(otherBounds.y + otherBounds.height - activeBounds.y);
                        }

                        guides.push_back(g);
                    }
                }
            }
        }
        for (const auto& child : node->getChildren()) traverse(child.get());
    };

    traverse(sceneRoot);
    return guides;
}

} // namespace vectma
