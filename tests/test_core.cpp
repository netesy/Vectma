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
};

void testSVGSerialization() {
    std::cout << "Testing SVG Serialization..." << std::endl;
    vectma::SceneGraph scene;
    scene.addChild(std::make_unique<vectma::RectNode>(10, 20, 100, 200));

    std::string svg = scene.toSVG();
    assert(svg.find("<svg") != std::string::npos);
    assert(svg.find("<rect") != std::string::npos);
    assert(svg.find("x=\"10.000000\"") != std::string::npos);
    assert(svg.find("width=\"100.000000\"") != std::string::npos);
    std::cout << "SVG serialization test passed." << std::endl;
}

void testPDFSerialization() {
    std::cout << "Testing PDF Serialization..." << std::endl;
    vectma::SceneGraph scene;
    std::string pdf = vectma::PDFExporter::exportToPDF(scene);
    assert(pdf.find("%PDF") != std::string::npos);
    std::cout << "PDF serialization test passed." << std::endl;
}

void testFigmaSerialization() {
    std::cout << "Testing Figma JSON Serialization..." << std::endl;
    vectma::SceneGraph scene;
    std::string json = vectma::FigmaExporter::exportToJSON(scene);
    assert(json.find("document") != std::string::npos);
    std::cout << "Figma JSON serialization test passed." << std::endl;
}

int main() {
    testSVGSerialization();
    testPDFSerialization();
    testFigmaSerialization();
    std::cout << "All Phase 10 Asset Exporter tests passed!" << std::endl;
    return 0;
}
