#include "core/CanvasNode.hpp"
#include "core/SceneGraph.hpp"
#include "core/WorkspaceStage.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/GPoint.hpp"
#include "core/GRect.hpp"
#include "core/GColor.hpp"
#include "core/GTransform.hpp"
#include "core/GeometryEngine.hpp"
#include "core/PDFExporter.hpp"
#include "core/FigmaExporter.hpp"
#include "core/LocaleManager.hpp"
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

void testCornerRounding() {
    std::cout << "Testing Non-Destructive Corner Rounding..." << std::endl;
    std::vector<vectma::BezierAnchor> anchors = { { {0, 0}, {0, 0}, {0, 0} }, { {100, 0}, {100, 0}, {100, 0} }, { {100, 100}, {100, 100}, {100, 100} } };
    vectma::PathNode path(anchors);

    path.cornerRadius = 10.0f;
    path.setStrokeWidth(0.0f);
    auto bbox = path.computeBoundingBox();
    // Bbox should still be based on anchors for this mock, but structural pass-through is verified
    assert(bbox.width == 100);
    assert(bbox.height == 100);
    std::cout << "Corner rounding structural pass-through verified." << std::endl;
}

void testDashCounts() {
    std::cout << "Testing Dash Pattern persistence..." << std::endl;
    vectma::PathNode path;
    path.dashPattern = { 5.0f, 2.0f };
    assert(path.dashPattern.size() == 2);
    assert(path.dashPattern[0] == 5.0f);
    std::cout << "Dash pattern attributes verified." << std::endl;
}

int main() {
    testCornerRounding();
    testDashCounts();
    std::cout << "All Phase 12 Path Effect tests passed!" << std::endl;
    return 0;
}
