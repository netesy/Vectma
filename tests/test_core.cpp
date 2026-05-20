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
#include "core/GeometryEngine.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class MockRenderPipeline : public vectma::RenderPipeline {
public:
    void beginFrame() override {}
    void endFrame() override {}
    void drawRect(const vectma::RectNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawEllipse(const vectma::EllipseNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawPath(const vectma::PathNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawBezierPath(const vectma::PathNode&) override {}
    void drawAnchorOverlay(const vectma::BezierAnchor&, bool, int) override {}
    void renderNode(const vectma::CanvasNode&) override {}
};

void testBooleanUnion() {
    std::cout << "Testing Boolean Union..." << std::endl;
    std::vector<vectma::BezierAnchor> a1 = { { {0, 0}, {0, 0}, {0, 0} }, { {50, 50}, {50, 50}, {50, 50} } };
    std::vector<vectma::BezierAnchor> a2 = { { {100, 100}, {100, 100}, {100, 100} } };

    vectma::PathNode p1(a1);
    vectma::PathNode p2(a2);

    auto result = vectma::GeometryEngine::combinePaths(p1, p2, vectma::BooleanOp::Union);
    assert(result->getAnchors().size() == 3);
    std::cout << "Boolean Union test passed." << std::endl;
}

void testBooleanIntersect() {
    std::cout << "Testing Boolean Intersect..." << std::endl;
    // Overlapping rects: [0,0, 100,100] and [50,50, 100,100]
    std::vector<vectma::BezierAnchor> a1 = { { {0, 0}, {0,0}, {0,0} }, { {100, 100}, {100,100}, {100,100} } };
    std::vector<vectma::BezierAnchor> a2 = { { {50, 50}, {50,50}, {50,50} }, { {150, 150}, {150,150}, {150,150} } };

    vectma::PathNode p1(a1);
    vectma::PathNode p2(a2);

    p1.setStrokeWidth(0);
    p2.setStrokeWidth(0);

    auto result = vectma::GeometryEngine::combinePaths(p1, p2, vectma::BooleanOp::Intersect);
    // In combinePaths Intersect, I used +/- 5 offset for handles. Let's fix that in GeometryEngine.cpp or here.
    // I'll fix it in GeometryEngine.cpp to not add arbitrary handle offsets for simple cases.

    result->setStrokeWidth(0);
    auto bbox = result->computeBoundingBox();

    std::cout << "bbox.x: " << bbox.x << ", bbox.y: " << bbox.y << ", bbox.w: " << bbox.width << ", bbox.h: " << bbox.height << std::endl;
    assert(bbox.x == 50);
    assert(bbox.y == 50);
    assert(bbox.width == 50);
    assert(bbox.height == 50);
    std::cout << "Boolean Intersect test passed." << std::endl;
}

int main() {
    testBooleanUnion();
    testBooleanIntersect();
    std::cout << "All CSG tests passed!" << std::endl;
    return 0;
}
