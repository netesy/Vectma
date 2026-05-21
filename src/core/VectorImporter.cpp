#include "core/VectorImporter.hpp"
#include "core/PathNode.hpp"

#ifdef VECTMA_USE_LUNASVG
#include <vendor/lunasvg/include/lunasvg.h>
#endif

#ifdef VECTMA_USE_PDFIUM
#include <vendor/pdfium/public/fpdfview.h>
#include <vendor/pdfium/public/fpdf_edit.h>
#endif

#include <iostream>

namespace vectma {

std::unique_ptr<SceneGraph> VectorImporter::ImportSVG(const std::string& svgContent) {
    auto scene = std::make_unique<SceneGraph>();
#ifdef VECTMA_USE_LUNASVG
    auto doc = lunasvg::Document::loadFromData(svgContent);
    if (!doc) return scene;
    // Implementation...
#else
    (void)svgContent;
#endif
    return scene;
}

std::unique_ptr<SceneGraph> VectorImporter::ImportPDF(const std::string& filePath) {
    auto scene = std::make_unique<SceneGraph>();
#ifdef VECTMA_USE_PDFIUM
    FPDF_LIBRARY_CONFIG config;
    config.version = 2;
    config.m_pUserFontPaths = NULL;
    config.m_pIsolate = NULL;
    config.m_v8EmbedderSlot = 0;
    FPDF_InitLibraryWithConfig(&config);

    FPDF_DOCUMENT doc = FPDF_LoadDocument(filePath.c_str(), NULL);
    if (!doc) {
        FPDF_DestroyLibrary();
        return scene;
    }

    int pageCount = FPDF_GetPageCount(doc);
    for (int i = 0; i < pageCount; ++i) {
        FPDF_PAGE page = FPDF_LoadPage(doc, i);
        if (page) {
            int objCount = FPDFPage_CountObjects(page);
            for (int j = 0; j < objCount; ++j) {
                FPDF_PAGEOBJECT obj = FPDFPage_GetObject(page, j);
                int type = FPDFPageObj_GetType(obj);
                (void)type;
            }
            FPDF_ClosePage(page);
        }
    }

    FPDF_CloseDocument(doc);
    FPDF_DestroyLibrary();
#else
    (void)filePath;
#endif
    return scene;
}

} // namespace vectma
