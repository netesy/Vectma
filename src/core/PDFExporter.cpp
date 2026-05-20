#include "core/PDFExporter.hpp"

namespace vectma {

std::string PDFExporter::exportToPDF(const SceneGraph& scene) {
    (void)scene;
    // Mock PDF generation logic
    std::string pdf = "%PDF-1.7\n";
    pdf += "1 0 obj << /Type /Catalog /Pages 2 0 R >> endobj\n";
    pdf += "%%EOF";
    return pdf;
}

} // namespace vectma
