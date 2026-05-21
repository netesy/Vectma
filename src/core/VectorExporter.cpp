#include "core/VectorExporter.hpp"

#ifdef VECTMA_USE_SKIA
#include "include/core/SkCanvas.h"
#include "include/docs/SkPDFDocument.h"
#include "include/svg/SkSVGCanvas.h"
#include "include/core/SkStream.h"

namespace vectma {

bool VectorExporter::ExportToPDF(const std::string& filePath, const SceneGraph& scene) {
    SkFILEWStream stream(filePath.c_str());
    auto doc = SkPDF::MakeDocument(&stream);
    if (!doc) return false;

    // For each page/artboard in scene
    SkCanvas* canvas = doc->beginPage(800, 600);
    // Draw scene to SkCanvas
    // scene.renderToSkia(canvas);
    doc->endPage();
    doc->close();
    return true;
}

bool VectorExporter::ExportToSVG(const std::string& filePath, const SceneGraph& scene) {
    SkFILEWStream stream(filePath.c_str());
    SkRect bounds = SkRect::MakeWH(800, 600);
    std::unique_ptr<SkCanvas> canvas = SkSVGCanvas::Make(bounds, &stream);
    if (!canvas) return false;

    // Draw scene to SkCanvas
    // scene.renderToSkia(canvas);
    return true;
}

} // namespace vectma

#else
namespace vectma {
bool VectorExporter::ExportToPDF(const std::string& filePath, const SceneGraph& scene) { (void)filePath; (void)scene; return false; }
bool VectorExporter::ExportToSVG(const std::string& filePath, const SceneGraph& scene) { (void)filePath; (void)scene; return false; }
}
#endif
