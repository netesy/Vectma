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
    void drawBezierPath(const vectma::PathNode&) override {}
    void drawAnchorOverlay(const vectma::BezierAnchor&, bool, int) override {}
    void renderNode(const vectma::CanvasNode& node) override {
        if (node.getClassName() == "MockNode") {
            callOrder.push_back(static_cast<const MockNode&>(node).id);
        }
    }
};

void testBezierMath() {
    std::cout << "Testing Bezier Math..." << std::endl;
    vectma::WorkspaceStage workspace;
    auto scene = std::make_shared<vectma::SceneGraph>();
    workspace.setScene(scene);

    std::vector<vectma::BezierAnchor> anchors;
    anchors.emplace_back(vectma::Point2D(100, 100), vectma::Point2D(80, 100), vectma::Point2D(120, 100), vectma::AnchorType::Symmetric);
    auto path = std::make_unique<vectma::PathNode>(anchors);
    vectma::PathNode* pathPtr = path.get();
    scene->addChild(std::move(path));

    workspace.addToSelection(pathPtr);
    workspace.setSubSelectionMode(true);

    // Hit test anchor position
    workspace.handleMouseDown({100, 100});
    assert(workspace.getActiveAnchorIndex() == 0);
    assert(workspace.getActiveHandleId() == 0);

    // Move anchor position
    workspace.handleMouseMove({110, 110});
    workspace.handleMouseUp();

    auto updatedAnchors = pathPtr->getAnchors();
    assert(updatedAnchors[0].position.x == 110);
    assert(updatedAnchors[0].position.y == 110);
    assert(updatedAnchors[0].handleIn.x == 90);
    assert(updatedAnchors[0].handleIn.y == 110); // Symmetric move moves handles too

    // Hit test handle
    workspace.handleMouseDown({90, 110});
    assert(workspace.getActiveAnchorIndex() == 0);
    assert(workspace.getActiveHandleId() == 1);

    // Move handle In, should mirror handle Out
    workspace.handleMouseMove({80, 110});
    workspace.handleMouseUp();

    updatedAnchors = pathPtr->getAnchors();
    assert(updatedAnchors[0].handleIn.x == 80);
    assert(updatedAnchors[0].handleOut.x == 140); // 110 + (110 - 80) = 140

    std::cout << "Bezier math tests passed." << std::endl;
}

void testPathHitTesting() {
    std::cout << "Testing Path Hit Testing..." << std::endl;
    std::vector<vectma::BezierAnchor> anchors;
    anchors.emplace_back(vectma::Point2D(100, 100), vectma::Point2D(80, 100), vectma::Point2D(120, 100));
    vectma::PathNode path(anchors);

    assert(path.hitTestAnchors({100, 100}, 5.0f) == 0); // anchor 0, pos
    assert(path.hitTestAnchors({80, 100}, 5.0f) == 1);  // anchor 0, handleIn
    assert(path.hitTestAnchors({120, 100}, 5.0f) == 2); // anchor 0, handleOut
    assert(path.hitTestAnchors({150, 150}, 5.0f) == -1);

    std::cout << "Path hit testing tests passed." << std::endl;
}

int main() {
    testBezierMath();
    testPathHitTesting();
    std::cout << "All new Bezier engine tests passed!" << std::endl;
    return 0;
}
