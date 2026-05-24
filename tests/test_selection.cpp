#include <cassert>
#include <iostream>
#include <cmath>
#include <chrono>
#include "ui/SelectionManager.hpp"
#include "core/RectNode.hpp"
#include "core/AlignmentEngine.hpp"
#include "core/SceneGraph.hpp"

using namespace vectma;

void testSelectionBounds() {
    SelectionManager sm;
    auto r1 = std::make_unique<RectNode>(10, 10, 100, 100);
    auto r2 = std::make_unique<RectNode>(200, 200, 50, 50);

    sm.add(r1.get());
    sm.add(r2.get());

    GRect bounds = sm.getSelectionBounds();

    assert(std::abs(bounds.x - 9.0) < 1e-6);
    assert(std::abs(bounds.y - 9.0) < 1e-6);
    assert(std::abs(bounds.width - 242.0) < 1e-6);
    assert(std::abs(bounds.height - 242.0) < 1e-6);

    std::cout << "testSelectionBounds passed" << std::endl;
}

void testDistribution() {
    SelectionManager sm;
    auto r1 = std::make_unique<RectNode>(0, 0, 100, 100);
    auto r2 = std::make_unique<RectNode>(150, 0, 100, 100);
    auto r3 = std::make_unique<RectNode>(500, 0, 100, 100);

    sm.add(r1.get());
    sm.add(r2.get());
    sm.add(r3.get());

    sm.distributeHorizontally();

    assert(std::abs(r1->getX() - 0.0) < 1e-6);
    assert(std::abs(r2->getX() - 250.0) < 1e-6);
    assert(std::abs(r3->getX() - 500.0) < 1e-6);

    std::cout << "testDistribution passed" << std::endl;
}

void testAlignmentEnginePerformance() {
    auto scene = std::make_shared<SceneGraph>();
    for (int i = 0; i < 1000; ++i) {
        scene->addChild(std::make_unique<RectNode>(i * 10, i * 10, 5, 5));
    }

    GRect activeBounds(100, 100, 50, 50);
    std::vector<CanvasNode*> ignored;

    auto start = std::chrono::high_resolution_clock::now();
    auto guides = AlignmentEngine::CalculateGuides(activeBounds, scene.get(), ignored);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "AlignmentEngine calculated " << guides.size() << " guides in " << duration << "us" << std::endl;

    // Threshold check for "below 1ms"
    assert(duration < 1000);

    std::cout << "testAlignmentEnginePerformance passed" << std::endl;
}

int main() {
    testSelectionBounds();
    testDistribution();
    testAlignmentEnginePerformance();
    return 0;
}
