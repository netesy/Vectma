#include "core/SceneGraph.hpp"
#include "core/RectNode.hpp"
#include "core/PathNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/GeometryEngine.hpp"
#include <cassert>
#include <iostream>

using namespace vectma;

void testCompoundShapeDirtyFlag() {
    std::cout << "Testing Compound Shape Dirty Flag Propagation..." << std::endl;
    auto compound = std::make_unique<CompoundShapeNode>(BooleanOpType::Union);
    auto rect = std::make_unique<RectNode>(0, 0, 100, 100);
    auto* rectPtr = rect.get();
    compound->addChild(std::move(rect));

    assert(compound->getChildren().size() == 1);
    assert(compound->isDirty());

    compound->setResolvedPath(std::make_unique<PathData>());
    assert(!compound->isDirty());

    rectPtr->setX(50);
    assert(compound->isDirty());

    std::cout << "Dirty Flag test passed." << std::endl;
}

void testGeometryEngineUnion() {
    std::cout << "Testing GeometryEngine::combinePaths (Union)..." << std::endl;
    PathNode p1({BezierAnchor({0,0}, {0,0}, {0,0}), BezierAnchor({100,0}, {100,0}, {100,0})});
    PathNode p2({BezierAnchor({200,0}, {200,0}, {200,0}), BezierAnchor({300,0}, {300,0}, {300,0})});
    auto result = GeometryEngine::combinePaths(p1, p2, BooleanOp::Union);
    assert(result->getCompiledPath().contours.size() == 2);
    std::cout << "Union test passed." << std::endl;
}

void testGeometryEngineSubtract() {
    std::cout << "Testing GeometryEngine::combinePaths (Subtract)..." << std::endl;
    // P1: Rect at 0,0, 100x100
    std::vector<BezierAnchor> a1 = {
        BezierAnchor({0,0}, {0,0}, {0,0}), BezierAnchor({100,0}, {100,0}, {100,0}),
        BezierAnchor({100,100}, {100,100}, {100,100}), BezierAnchor({0,100}, {0,100}, {0,100})
    };
    PathNode p1(a1); p1.setClosed(true);

    // P2: Rect at 200,200, 50x50 (disjoint)
    std::vector<BezierAnchor> a2 = {
        BezierAnchor({200,200}, {200,200}, {200,200}), BezierAnchor({250,200}, {250,200}, {250,200}),
        BezierAnchor({250,250}, {250,250}, {250,250}), BezierAnchor({200,250}, {200,250}, {200,250})
    };
    PathNode p2(a2); p2.setClosed(true);

    auto result = GeometryEngine::combinePaths(p1, p2, BooleanOp::Subtract);
    // Should keep P1 because P2 is disjoint
    assert(result->getCompiledPath().contours.size() == 1);

    // P3: Rect at 50,50, 20x20 (contained in P1)
    std::vector<BezierAnchor> a3 = {
        BezierAnchor({50,50}, {50,50}, {50,50}), BezierAnchor({70,50}, {70,50}, {70,50}),
        BezierAnchor({70,70}, {70,70}, {70,70}), BezierAnchor({50,70}, {50,70}, {50,70})
    };
    PathNode p3(a3); p3.setClosed(true);

    auto result2 = GeometryEngine::combinePaths(p1, p3, BooleanOp::Subtract);
    // In our analytical fallback, if P3 is inside P1, it's removed from result (kept as target minus overlap)
    // Here our simple containment check skips contours that are fully inside source.
    // P1 is NOT inside P3, so P1 is kept.
    assert(result2->getCompiledPath().contours.size() == 1);

    std::cout << "Subtract test passed." << std::endl;
}

int main() {
    testCompoundShapeDirtyFlag();
    testGeometryEngineUnion();
    testGeometryEngineSubtract();
    std::cout << "All Boolean tests passed!" << std::endl;
    return 0;
}
