#include "prototype/InteractionGraph.hpp"
#include "prototype/PrototypePreviewStage.hpp"
#include "core/SceneGraph.hpp"
#include "core/ArtboardNode.hpp"
#include "core/RectNode.hpp"
#include "geometry/BezierMath.hpp"
#include <cassert>
#include <iostream>

using namespace vectma;

void test_interaction_graph() {
    auto& ig = InteractionGraph::getInstance();
    NodeId src = {100, 1, 1};
    NodeId target = {200, 1, 2};

    PrototypeLink link;
    link.sourceNodeId = src;
    link.targetArtboardId = target;
    link.trigger = TriggerType::MouseClick;

    ig.addLink(link);

    auto found = ig.findLink(src, TriggerType::MouseClick);
    assert(found != nullptr);
    assert(found->targetArtboardId == target);

    ig.onNodeDeleted(src);
    assert(ig.findLink(src, TriggerType::MouseClick) == nullptr);
    std::cout << "test_interaction_graph passed." << std::endl;
}

void test_bezier_easing() {
    float coeffs[4] = {0.25f, 0.1f, 0.25f, 1.0f}; // Ease
    double val0 = geometry::BezierEvaluator::evaluateEasing(coeffs, 0.0);
    double val1 = geometry::BezierEvaluator::evaluateEasing(coeffs, 1.0);
    double valMid = geometry::BezierEvaluator::evaluateEasing(coeffs, 0.5);

    assert(std::abs(val0 - 0.0) < 1e-6);
    assert(std::abs(val1 - 1.0) < 1e-6);
    assert(valMid > 0.0 && valMid < 1.0);
    std::cout << "test_bezier_easing passed." << std::endl;
}

void test_interaction_integrity() {
    auto& ig = InteractionGraph::getInstance();
    NodeId src = {300, 1, 1};
    NodeId target = {400, 1, 1};

    PrototypeLink link;
    link.sourceNodeId = src;
    link.targetArtboardId = target;
    ig.addLink(link);

    // Test target deletion
    ig.onNodeDeleted(target);
    assert(ig.getLinksBySource(src).empty());
    std::cout << "test_interaction_integrity (target deletion) passed." << std::endl;
}

int main() {
    test_interaction_graph();
    test_bezier_easing();
    test_interaction_integrity();
    return 0;
}
