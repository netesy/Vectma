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
    void drawRect(const vectma::RectNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawEllipse(const vectma::EllipseNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawPath(const vectma::PathNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawBezierPath(const vectma::PathNode&) override {}
    void drawAnchorOverlay(const vectma::BezierAnchor&, bool, int) override {}
    void renderNode(const vectma::CanvasNode& node) override {
        if (node.getClassName() == "MockNode") {
            callOrder.push_back(static_cast<const MockNode&>(node).id);
        }
    }
};

void testGradients() {
    std::cout << "Testing Gradients..." << std::endl;
    vectma::RectNode rect(0, 0, 100, 100);
    rect.setFillType(vectma::FillType::LinearGradient);

    auto config = rect.getGradientConfig();
    config.stops.push_back({0.5f, 0xFF0000FF});
    rect.setGradientConfig(config);

    assert(rect.getGradientConfig().stops.size() == 3);
    assert(rect.getGradientConfig().stops[2].offset == 0.5f);
    std::cout << "Gradient tests passed." << std::endl;
}

void testStrokeAlignment() {
    std::cout << "Testing Stroke Alignment BBox..." << std::endl;
    vectma::RectNode rect(10, 10, 100, 100);
    rect.setStrokeWidth(10.0);

    // Center alignment
    rect.setStrokeAlignment(vectma::StrokeAlignment::Center);
    auto bbox = rect.computeBoundingBox();
    // x: 10 - 5 = 5, y: 10 - 5 = 5, w: 100 + 10 = 110, h: 100 + 10 = 110
    assert(bbox.x == 5 && bbox.y == 5 && bbox.width == 110 && bbox.height == 110);

    // Outside alignment
    rect.setStrokeAlignment(vectma::StrokeAlignment::Outside);
    bbox = rect.computeBoundingBox();
    // x: 10 - 10 = 0, y: 10 - 10 = 0, w: 100 + 20 = 120, h: 100 + 20 = 120
    assert(bbox.x == 0 && bbox.y == 0 && bbox.width == 120 && bbox.height == 120);

    // Inside alignment
    rect.setStrokeAlignment(vectma::StrokeAlignment::Inside);
    bbox = rect.computeBoundingBox();
    assert(bbox.x == 10 && bbox.y == 10 && bbox.width == 100 && bbox.height == 100);

    std::cout << "Stroke alignment tests passed." << std::endl;
}

int main() {
    testGradients();
    testStrokeAlignment();
    std::cout << "All Phase 7 tests passed!" << std::endl;
    return 0;
}
