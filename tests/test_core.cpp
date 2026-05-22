#include "core/CanvasNode.hpp"
#include "core/SceneGraph.hpp"
#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
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
#include "core/LayerNode.hpp"
#include "core/ArtboardNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
#include <chrono>
#include <cmath>

class MockRenderPipeline : public vectma::RenderPipeline {
public:
    void beginFrame() override {}
    void endFrame() override {}
    void drawRect(const vectma::RectNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawEllipse(const vectma::EllipseNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawPath(const vectma::PathNode&, vectma::FillType, const vectma::GradientConfig&, vectma::StrokeAlignment) override {}
    void drawText(const vectma::TextNode&) override {}
    void drawImage(const vectma::ImageNode&) override {}
    void drawBezierPath(const vectma::PathNode&) override {}
    void drawAnchorOverlay(const vectma::BezierAnchor&, bool, int) override {}
    void renderNode(const vectma::CanvasNode&) override {}
    std::vector<uint8_t> exportRaster(float) override { return {}; }
    void setStrokeStyle(const std::vector<float>&, float) override {}
    void drawSnappingGuide(const vectma::Point2D&, const vectma::Point2D&) override {}

    void pushClipRect(const vectma::GRect&) override {}
    void popClipRect() override {}
    void setGlobalOpacity(float) override {}
};

void testSnapping() {
    std::cout << "Testing Precision Snapping Engine..." << std::endl;
    auto scene = std::make_shared<vectma::SceneGraph>();
    auto rect = std::make_unique<vectma::RectNode>(100, 100, 50, 50);
    rect->setStrokeWidth(0);
    scene->addChild(std::move(rect));

    vectma::Point2D cursor(105, 105);
    auto snap = vectma::SnappingEngine::findSnapPoint(cursor, *scene, 10.0f);

    assert(snap.has_value());
    assert(snap->snappedPoint.x == 100.0);
    assert(snap->snappedPoint.y == 100.0);
    std::cout << "Snapping accuracy test passed." << std::endl;
}

void testModifiers() {
    std::cout << "Testing Non-Destructive Modifier Pipeline..." << std::endl;
    std::vector<vectma::BezierAnchor> anchors = {
        vectma::BezierAnchor({0, 0}, {0, 0}, {0, 0}),
        vectma::BezierAnchor({100, 0}, {100, 0}, {100, 0}),
        vectma::BezierAnchor({100, 100}, {100, 100}, {100, 100})
    };
    vectma::PathNode path(anchors);
    path.setClosed(true);

    auto roundMod = std::make_unique<vectma::CornerRoundingModifier>(10.0f);
    auto roundModPtr = roundMod.get();
    path.addModifier(std::move(roundMod));

    assert(path.getCompiledPath().contours[0].anchors.size() == 6);
    roundModPtr->setRadius(20.0f);
    assert(roundModPtr->isDirty());
    assert(path.getCompiledPath().contours[0].anchors.size() == 6);
    std::cout << "Modifier Pipeline tests passed." << std::endl;
}

void testLayersAndArtboards() {
    std::cout << "Testing Enhanced Layer Management & Artboards..." << std::endl;

    auto layer = std::make_unique<vectma::LayerNode>("RootLayer");
    auto rect = std::make_unique<vectma::RectNode>(10, 10, 50, 50);
    auto rectPtr = rect.get();
    layer->addChild(std::move(rect));

    assert(layer->getChildren().size() == 1);
    assert(rectPtr->getParent() == layer.get());

    layer->setOpacity(0.5f);
    assert(layer->getOpacity() == 0.5f);

    auto rect2 = std::make_unique<vectma::RectNode>(20, 20, 50, 50);
    auto rect2Ptr = rect2.get();
    layer->addChild(std::move(rect2));

    assert(layer->getChildren().size() == 2);
    assert(layer->getChildren()[0].get() == rectPtr);
    assert(layer->getChildren()[1].get() == rect2Ptr);

    rectPtr->bringToFront();
    assert(layer->getChildren()[0].get() == rect2Ptr);
    assert(layer->getChildren()[1].get() == rectPtr);

    rectPtr->sendToBack();
    assert(layer->getChildren()[0].get() == rectPtr);
    assert(layer->getChildren()[1].get() == rect2Ptr);

    vectma::ArtboardNode artboard("MainArtboard", vectma::GRect(100, 100, 500, 500));
    assert(artboard.computeBoundingBox().x == 100);
    assert(artboard.computeBoundingBox().width == 500);

    std::cout << "Layer and Artboard tests passed." << std::endl;
}

void testStatusReporting() {
    std::cout << "Testing Workspace Status Reporting..." << std::endl;

    vectma::WorkspaceStage stage;
    auto scene = std::make_shared<vectma::SceneGraph>();
    stage.setScene(scene);

    scene->addChild(std::make_unique<vectma::RectNode>(0, 0, 100, 100));
    assert(stage.getSceneNodeCount() == 2);

    stage.setViewMatrix(vectma::GTransform(2.0, 0, 0, 2.0, 0, 0));
    assert(std::abs(stage.getScale() - 2.0) < 1e-6);

    stage.handleMouseMove(vectma::Point2D(100, 100));
    assert(std::abs(stage.getCursorCanvasPos().x - 50.0) < 1e-6);

    std::cout << "Status Reporting tests passed." << std::endl;
}

void testUIState() {
    std::cout << "Testing UI State tracking..." << std::endl;
    vectma::WorkspaceStage stage;

    stage.setTool(vectma::ToolType::Brush);
    assert(stage.getTool() == vectma::ToolType::Brush);

    stage.setBrushSize(25.5f);
    assert(std::abs(stage.getBrushSize() - 25.5f) < 1e-6);

    stage.setBrushBleeding(0.75f);
    assert(std::abs(stage.getBrushBleeding() - 0.75f) < 1e-6);

    stage.setFontFamily("Poppins");
    assert(stage.getFontFamily() == "Poppins");

    stage.setFontSize(42.0f);
    assert(std::abs(stage.getFontSize() - 42.0f) < 1e-6);

    std::cout << "UI State tests passed." << std::endl;
}

int main() {
    testSnapping();
    testModifiers();
    testLayersAndArtboards();
    testStatusReporting();
    testUIState();
    std::cout << "All Phase 19 tests passed!" << std::endl;
    return 0;
}
