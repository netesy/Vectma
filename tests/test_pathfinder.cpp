#include "geometry/PathfinderEngine.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/PathNode.hpp"
#include "core/RectNode.hpp"
#include <cassert>
#include <iostream>
#include <vector>

using namespace vectma;
using namespace vectma::geometry;

void test_rdp_optimization() {
    std::cout << "Testing RDP Optimization..." << std::endl;
    auto node = std::make_unique<PathNode>();
    std::vector<BezierAnchor> anchors;
    for (int i = 0; i <= 10; ++i) {
        anchors.emplace_back(Point2D{(double)i * 10, 0}, Point2D{(double)i * 10, 0}, Point2D{(double)i * 10, 0});
    }
    node->setBaseContours({Contour(anchors, false)});
    assert(node->getCompiledPath().contours[0].anchors.size() == 11);
    PathfinderEngine::Optimize(node.get(), 1.0);
    assert(node->getCompiledPath().contours[0].anchors.size() == 2);
    std::cout << "RDP Optimization Passed!" << std::endl;
}

void test_flattening_basic() {
    std::cout << "Testing Pathfinder Flattening..." << std::endl;
    auto parent = std::make_unique<RectNode>(0, 0, 1000, 1000);
    auto compound = std::make_unique<CompoundShapeNode>(BooleanOpType::Union);
    auto r1 = std::make_unique<RectNode>(0, 0, 100, 100);
    auto r2 = std::make_unique<RectNode>(50, 50, 100, 100);
    compound->addChild(std::move(r1));
    compound->addChild(std::move(r2));

    CompoundShapeNode* compoundPtr = compound.get();
    parent->addChild(std::move(compound));

    PathfinderEngine::Flatten(compoundPtr);

    const auto& children = parent->getChildren();
    assert(!children.empty());
    std::string className = children[0]->getClassName();
    std::cout << "Class name after flatten: " << className << std::endl;
    assert(className == "PathNode");
    std::cout << "Flattening Basic Passed!" << std::endl;
}

void test_stroke_to_path() {
    std::cout << "Testing Stroke to Path..." << std::endl;
    auto parent = std::make_unique<RectNode>(0, 0, 1000, 1000);
    auto node = std::make_unique<RectNode>(0, 0, 100, 100);
    node->setStrokeWidth(10.0);
    CanvasNode* nodePtr = node.get();
    parent->addChild(std::move(node));

    PathfinderEngine::ConvertStrokeToPath(nodePtr);

    assert(parent->getChildren().size() == 2);
    assert(parent->getChildren()[1]->getClassName() == "PathNode");
    std::cout << "Stroke to Path Passed!" << std::endl;
}

int main() {
    test_rdp_optimization();
    test_flattening_basic();
    test_stroke_to_path();
    std::cout << "All Pathfinder tests passed!" << std::endl;
    return 0;
}
