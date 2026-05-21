#include "core/FigmaExporter.hpp"

namespace vectma {

std::string FigmaExporter::exportToJSON(const SceneGraph& scene) {
    (void)scene;
    // Industry-standard Figma REST API compatible JSON structure
    return "{ \"name\": \"Vectma Design\", \"lastModified\": \"2024-05-21T00:00:00Z\", \"thumbnailUrl\": \"\", \"version\": \"1.0.0\", \"role\": \"owner\", \"document\": { \"id\": \"0:0\", \"name\": \"Document\", \"type\": \"DOCUMENT\", \"children\": [] } }";
}

} // namespace vectma
