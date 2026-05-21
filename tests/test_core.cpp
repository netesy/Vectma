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
    void drawText(const vectma::TextNode&) override {}
    void drawBezierPath(const vectma::PathNode&) override {}
    void drawAnchorOverlay(const vectma::BezierAnchor&, bool, int) override {}
    void renderNode(const vectma::CanvasNode&) override {}
    std::vector<uint8_t> exportRaster(float) override { return {}; }
    void setStrokeStyle(const std::vector<float>&, float) override {}
};

void testSymbols() {
    std::cout << "Testing Component Symbols and Instances..." << std::endl;
    auto& registry = vectma::SymbolRegistry::getInstance();

    auto master = std::make_unique<vectma::RectNode>(0, 0, 100, 100);
    master->setStrokeWidth(0.0f);
    registry.registerSymbol("Button", std::move(master));

    assert(registry.getSymbolRoot("Button") != nullptr);

    vectma::InstanceNode instance("Button");
    auto bbox = instance.computeBoundingBox();
    assert(bbox.width == 100);
    assert(bbox.height == 100);

    instance.setOverride("text", "Click Me");
    assert(instance.getOverride("text") == "Click Me");

    std::cout << "Symbol and Instance tests passed." << std::endl;
}

int main() {
    testSymbols();
    std::cout << "All Phase 14 Symbol tests passed!" << std::endl;
    return 0;
}
