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
};

void testSpatialPerformance() {
    std::cout << "Testing Spatial Indexing Performance (10,000 nodes)..." << std::endl;
    auto scene = std::make_shared<vectma::SceneGraph>();

    for (int i = 0; i < 10000; ++i) {
        scene->addChild(std::make_unique<vectma::RectNode>(i, i, 10, 10));
    }

    vectma::WorkspaceStage workspace;
    workspace.setScene(scene);
    workspace.setTool(vectma::ToolType::Select);

    auto start = std::chrono::high_resolution_clock::now();
    workspace.handleMouseDown({5000, 5000}); // Should be O(log N)
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Spatial pick duration: " << duration << "us" << std::endl;

    assert(workspace.getSelection().size() == 1);
    std::cout << "Spatial performance test passed." << std::endl;
}

int main() {
    testSpatialPerformance();
    std::cout << "All Phase 15 Performance tests passed!" << std::endl;
    return 0;
}
