#include <cassert>
#include <iostream>
#include <cmath>
#include "core/ArtboardNode.hpp"
#include "core/RectNode.hpp"
#include "core/WorkspaceStage.hpp"
#include "core/SceneGraph.hpp"
#include "renderer/RenderPipeline.hpp"

using namespace vectma;

class MockRenderer : public BaselineRenderer {
public:
    int pushTransformCount = 0;
    int popTransformCount = 0;
    int pushClipCount = 0;
    int popClipCount = 0;

    void pushTransform(const GTransform& t) override { (void)t; pushTransformCount++; }
    void popTransform() override { popTransformCount++; }
    void pushClipRect(const GRect& r) override { (void)r; pushClipCount++; }
    void popClipRect() override { popClipCount++; }
};

void testArtboardClippingAndRelative() {
    auto artboard = std::make_unique<ArtboardNode>("Mobile", GRect(100, 100, 400, 800));
    auto rect = std::make_unique<RectNode>(10, 10, 50, 50);
    artboard->addChild(std::move(rect));

    MockRenderer renderer;
    artboard->render(renderer);

    assert(renderer.pushTransformCount == 1);
    assert(renderer.pushClipCount == 1);

    // Check hit testing with relative coordinates
    // Global point (115, 115) is local (15, 15) in artboard (100, 100)
    // Rect is at local (10, 10) with size 50x50, so (15, 15) should hit it.
    assert(artboard->containsPoint(GPoint(115, 115)) == true);
    // Outside artboard
    assert(artboard->containsPoint(GPoint(50, 50)) == false);
    // Outside rect but inside artboard
    assert(artboard->containsPoint(GPoint(105, 105)) == true); // It hits the artboard background area

    std::cout << "testArtboardClippingAndRelative passed" << std::endl;
}

void testWaypointInterpolation() {
    WorkspaceStage stage;
    stage.setViewportSize(1000, 1000);

    // Set current view to (500,500) zoom 1.0 (Identity matrix means canvas origin at top-left of viewport)
    // Actually Identity matrix: viewport(0,0) -> canvas(0,0)
    // If viewport size is 1000x1000, then viewport center is (500,500) -> canvas(500,500)
    stage.setViewMatrix(GTransform::Identity());

    auto& wm = stage.getWaypointManager();
    wm.saveWaypoint(0, "Target", 1000, 1000, 2.0);

    stage.jumpToWaypoint(0);

    // Initial state check
    Point2D center = stage.screenToCanvas(Point2D(500, 500));
    assert(std::abs(center.x - 500) < 1e-5);
    assert(std::abs(center.y - 500) < 1e-5);

    // Tick 0.15s (halfway 300ms)
    // easedT = 3*(0.5)^2 - 2*(0.5)^3 = 0.75 - 0.25 = 0.5
    stage.tick(0.15);

    Point2D midCenter = stage.screenToCanvas(Point2D(500, 500));
    // Start (500, 500), Target (1000, 1000), t=0.5 -> expected (750, 750)
    assert(std::abs(midCenter.x - 750) < 1e-5);
    assert(std::abs(midCenter.y - 750) < 1e-5);
    assert(std::abs(stage.getScale() - 1.5) < 1e-5);

    // Tick another 0.15s (completion)
    stage.tick(0.15);
    Point2D endCenter = stage.screenToCanvas(Point2D(500, 500));
    assert(std::abs(endCenter.x - 1000) < 1e-5);
    assert(std::abs(endCenter.y - 1000) < 1e-5);
    assert(std::abs(stage.getScale() - 2.0) < 1e-5);

    std::cout << "testWaypointInterpolation passed" << std::endl;
}

int main() {
    testArtboardClippingAndRelative();
    testWaypointInterpolation();
    return 0;
}
