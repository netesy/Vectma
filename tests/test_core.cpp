#include "core/CanvasNode.hpp"
#include "core/SceneGraph.hpp"
#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/InstanceNode.hpp"
#include "core/SymbolRegistry.hpp"
#include "core/GPoint.hpp"
#include "core/GRect.hpp"
#include "core/GColor.hpp"
#include "core/GTransform.hpp"
#include "core/GeometryEngine.hpp"
#include "core/PDFExporter.hpp"
#include "core/FigmaExporter.hpp"
#include "core/LocaleManager.hpp"
#include "core/HistoryManager.hpp"
#include "core/Commands.hpp"
#include "core/snap/SnappingEngine.hpp"
#include "core/modifiers/CornerRoundingModifier.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>

class MockRenderPipeline : public vectma::RenderPipeline {
public:
    void beginFrame() override {}
    void endFrame() override {}
    void drawRect(const vectma::RectNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawEllipse(const vectma::EllipseNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawPath(const vectma::PathNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawText(const vectma::TextNode&) override {}
    void drawBezierPath(const vectma::PathNode&) override {}
    void drawAnchorOverlay(const vectma::BezierAnchor&, bool, int) override {}
    void renderNode(const vectma::CanvasNode&) override {}
    std::vector<uint8_t> exportRaster(float) override { return {}; }
    void setStrokeStyle(const std::vector<float>&, float) override {}
    void drawSnappingGuide(const vectma::Point2D&, const vectma::Point2D&) override {}
};

void testSnapping() {
    std::cout << "Testing Precision Snapping Engine..." << std::endl;
    auto scene = std::make_shared<vectma::SceneGraph>();
    // Add a rect at (100, 100)
    auto rect = std::make_unique<vectma::RectNode>(100, 100, 50, 50);
    rect->setStrokeWidth(0); // For exact math
    scene->addChild(std::move(rect));

    // Cursor near (100, 100)
    vectma::Point2D cursor(105, 105);
    auto snap = vectma::SnappingEngine::findSnapPoint(cursor, *scene, 10.0f);

    assert(snap.has_value());
    assert(snap->snappedPoint.x == 100.0);
    assert(snap->snappedPoint.y == 100.0);
    assert(snap->snappedX && snap->snappedY);

    std::cout << "Snapping accuracy test passed." << std::endl;
}

void testModifiers() {
    std::cout << "Testing Non-Destructive Modifier Pipeline..." << std::endl;

    std::vector<vectma::BezierAnchor> anchors = {
        vectma::BezierAnchor({0, 0}, {0, 0}, {0, 0}),
        vectma::BezierAnchor({100, 0}, {100, 0}, {100, 0})
    };
    vectma::PathNode path(anchors);

    assert(path.getCompiledPath().anchors.size() == 2);

    auto roundMod = std::make_unique<vectma::CornerRoundingModifier>(10.0f);
    auto roundModPtr = roundMod.get();
    path.addModifier(std::move(roundMod));

    // Evaluation
    assert(path.getCompiledPath().anchors.size() == 2);
    assert(!roundModPtr->isDirty());

    // Mutation
    roundModPtr->setRadius(20.0f);
    assert(roundModPtr->isDirty());

    // Cache invalidation & re-evaluation
    assert(path.getCompiledPath().anchors.size() == 2);
    assert(!roundModPtr->isDirty());

    std::cout << "Modifier Pipeline tests passed." << std::endl;
}

int main() {
    testSnapping();
    testModifiers();
    std::cout << "All Phase 17 tests passed!" << std::endl;
    return 0;
}
