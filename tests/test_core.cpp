#include "core/CanvasNode.hpp"
#include "core/SceneGraph.hpp"
#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/GPoint.hpp"
#include "core/GRect.hpp"
#include "core/GColor.hpp"
#include "core/GTransform.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class MockNode : public vectma::CanvasNode {
public:
    std::string id;
    MockNode(std::string name) : id(name) {}
    std::string getClassName() const override { return "MockNode"; }
    void render(vectma::RenderPipeline& p) const override { p.renderNode(*this); }
    bool containsPoint(const vectma::GPoint&) const override { return false; }
    vectma::GRect computeBoundingBox() const override { return vectma::GRect(0, 0, 0, 0); }
};

class MockRenderPipeline : public vectma::RenderPipeline {
public:
    std::vector<std::string> callOrder;
    void beginFrame() override {}
    void endFrame() override {}
    void drawRect(const vectma::RectNode&) override {}
    void drawEllipse(const vectma::EllipseNode&) override {}
    void drawPath(const vectma::PathNode&) override {}
    void renderNode(const vectma::CanvasNode& node) override {
        if (node.getClassName() == "MockNode") {
            callOrder.push_back(static_cast<const MockNode&>(node).id);
        }
    }
};

void testToolSwitching() {
    std::cout << "Testing Tool Switching..." << std::endl;
    vectma::WorkspaceStage workspace;
    workspace.setTool(vectma::ToolType::Rect);
    assert(workspace.getTool() == vectma::ToolType::Rect);

    workspace.setTool(vectma::ToolType::Ellipse);
    assert(workspace.getTool() == vectma::ToolType::Ellipse);
    std::cout << "Tool switching tests passed." << std::endl;
}

void testMarqueeSelection() {
    std::cout << "Testing Marquee Selection..." << std::endl;
    auto workspace = std::make_unique<vectma::WorkspaceStage>();
    auto scene = std::make_shared<vectma::SceneGraph>();
    workspace->setScene(scene);

    scene->addChild(std::make_unique<vectma::RectNode>(10, 10, 20, 20)); // Node 0
    scene->addChild(std::make_unique<vectma::RectNode>(50, 50, 20, 20)); // Node 1

    workspace->setTool(vectma::ToolType::Marquee);

    // Simulate drag selecting both nodes
    workspace->handleMouseDown({0, 0});
    workspace->handleMouseMove({100, 100});
    workspace->handleMouseUp();

    assert(workspace->getSelection().size() == 2);

    // Simulate drag selecting only first node
    workspace->handleMouseDown({0, 0});
    workspace->handleMouseMove({40, 40});
    workspace->handleMouseUp();

    assert(workspace->getSelection().size() == 1);
    std::cout << "Marquee selection tests passed." << std::endl;
}

void testShapeCreation() {
    std::cout << "Testing Shape Creation Tool..." << std::endl;
    auto workspace = std::make_unique<vectma::WorkspaceStage>();
    auto scene = std::make_shared<vectma::SceneGraph>();
    workspace->setScene(scene);

    workspace->setTool(vectma::ToolType::Rect);

    workspace->handleMouseDown({10, 10});
    workspace->handleMouseMove({60, 60});
    workspace->handleMouseUp();

    assert(scene->getChildren().size() == 1);
    assert(scene->getChildren()[0]->getClassName() == "RectNode");

    auto bbox = scene->getChildren()[0]->computeBoundingBox();
    assert(bbox.x == 10 && bbox.width == 50);

    std::cout << "Shape creation tests passed." << std::endl;
}

void testCoordinateTranslation() {
    std::cout << "Testing Coordinate Translation..." << std::endl;
    vectma::WorkspaceStage workspace;
    // View translated by 100, 100
    workspace.setViewMatrix(vectma::GTransform::Translation(100, 100));

    vectma::GPoint screenPos(150, 150);
    vectma::GPoint canvasPos = workspace.screenToCanvas(screenPos);

    assert(canvasPos.x == 50);
    assert(canvasPos.y == 50);
    std::cout << "Coordinate translation tests passed." << std::endl;
}

int main() {
    testToolSwitching();
    testMarqueeSelection();
    testShapeCreation();
    testCoordinateTranslation();
    std::cout << "All core tests passed!" << std::endl;
    return 0;
}
