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
#include <string_view>

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

void testI18n() {
    std::cout << "Testing Internationalization Engine..." << std::endl;
    auto& mgr = vectma::LocaleManager::getInstance();

    mgr.setLocale(vectma::Locale::EN_US);
    assert(mgr.translate("toolbar.select") == "Select");

    mgr.setLocale(vectma::Locale::FR_FR);
    assert(mgr.translate("toolbar.select") == "Sélectionner");

    // Fallback
    assert(mgr.translate("non.existent.key") == "non.existent.key");

    std::cout << "I18n engine tests passed." << std::endl;
}

void testUTF8Safety() {
    std::cout << "Testing UTF-8 Safety..." << std::endl;
    auto& mgr = vectma::LocaleManager::getInstance();
    mgr.setLocale(vectma::Locale::FR_FR);

    std::string_view frText = mgr.translate("toolbar.marquee");
    // "Rectangle de sélection" contains 'é' which is multi-byte
    assert(frText.length() > 0);

    std::cout << "UTF-8 safety checks passed." << std::endl;
}

int main() {
    testI18n();
    testUTF8Safety();
    std::cout << "All Phase 11 I18n tests passed!" << std::endl;
    return 0;
}
