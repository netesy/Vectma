#include "core/PathNode.hpp"
#include "core/modifiers/CornerRoundingModifier.hpp"
#include "core/modifiers/DashGeneratorModifier.hpp"
#include <cassert>
#include <iostream>

using namespace vectma;

void testModifierStack() {
    std::cout << "Testing Modifier Stack..." << std::endl;

    std::vector<BezierAnchor> anchors = {
        BezierAnchor({0, 0}, {0, 0}, {0, 0}),
        BezierAnchor({100, 0}, {100, 0}, {100, 0}),
        BezierAnchor({100, 100}, {100, 100}, {100, 100})
    };
    PathNode path(anchors);
    path.setClosed(true);

    // Initial state
    assert(path.getCompiledPath().contours.size() == 1);
    assert(path.getCompiledPath().contours[0].anchors.size() == 3);

    // Add modifier
    auto roundMod = std::make_unique<CornerRoundingModifier>(10.0f);
    auto roundModPtr = roundMod.get();
    path.addModifier(std::move(roundMod));

    // Evaluation
    auto compiled = path.getCompiledPath();
    assert(compiled.contours.size() == 1);
    // 3 anchors -> 6 anchors when rounded
    assert(compiled.contours[0].anchors.size() == 6);
    assert(!roundModPtr->isDirty());

    // Dirtying modifier
    roundModPtr->setRadius(20.0f);
    assert(roundModPtr->isDirty());

    // Cache should re-evaluate
    assert(path.getCompiledPath().contours[0].anchors.size() == 6);
    assert(!roundModPtr->isDirty());

    std::cout << "Modifier Stack tests passed." << std::endl;
}

int main() {
    testModifierStack();
    return 0;
}
