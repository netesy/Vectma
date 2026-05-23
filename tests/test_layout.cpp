#include "core/SceneGraph.hpp"
#include "core/RectNode.hpp"
#include "layout/LayoutSolver.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

using namespace vectma;

void testBasicAutoLayout() {
    std::cout << "Testing Basic Auto-Layout..." << std::endl;

    // Set client ID to ensure tick() works
    LamportClock::getInstance().setClientId(1);

    auto container = std::make_unique<RectNode>(0, 0, 500, 500);
    container->setStrokeWidth(0);

    LayoutProperties props;
    props.enabled = true;
    props.direction = LayoutDirection::Horizontal;
    props.gap = 10.0f;
    props.padding = {0, 0, 0, 0};
    container->setLayoutProps(props);

    auto rect1 = std::make_unique<RectNode>(0, 0, 100, 100);
    rect1->setStrokeWidth(0);
    auto rect2 = std::make_unique<RectNode>(0, 0, 100, 100);
    rect2->setStrokeWidth(0);

    container->addChild(std::move(rect1));
    container->addChild(std::move(rect2));

    LayoutSolver::ResolveConstraints(container.get());

    auto& children = container->getChildren();
    RectNode* r1 = dynamic_cast<RectNode*>(children[0].get());
    RectNode* r2 = dynamic_cast<RectNode*>(children[1].get());

    std::cout << "R1 X: " << r1->getX() << " Y: " << r1->getY() << std::endl;
    std::cout << "R2 X: " << r2->getX() << " Y: " << r2->getY() << std::endl;

    if (std::abs(r1->getX() - 0.0) > 1e-6 || std::abs(r2->getX() - 110.0) > 1e-6) {
        std::cerr << "Basic Auto-Layout FAILED!" << std::endl;
        exit(1);
    }

    std::cout << "Basic Auto-Layout passed." << std::endl;
}

void testFillContainer() {
    std::cout << "Testing Fill Container..." << std::endl;
    auto container = std::make_unique<RectNode>(0, 0, 500, 500);
    container->setStrokeWidth(0);

    LayoutProperties props;
    props.enabled = true;
    props.direction = LayoutDirection::Vertical;
    props.gap = 0.0f;
    container->setLayoutProps(props);

    auto rect1 = std::make_unique<RectNode>(0, 0, 100, 100);
    rect1->setStrokeWidth(0);
    rect1->setVerticalSizing(SizingRule::FillContainer);

    container->addChild(std::move(rect1));
    LayoutSolver::ResolveConstraints(container.get());

    RectNode* r1 = dynamic_cast<RectNode*>(container->getChildren()[0].get());
    std::cout << "R1 Height: " << r1->getHeight() << std::endl;
    if (std::abs(r1->getHeight() - 500.0) > 1e-6) {
        std::cerr << "Fill Container FAILED!" << std::endl;
        exit(1);
    }

    std::cout << "Fill Container passed." << std::endl;
}

int main() {
    testBasicAutoLayout();
    testFillContainer();
    std::cout << "All Layout tests passed!" << std::endl;
    return 0;
}
