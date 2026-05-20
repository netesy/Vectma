#include "core/FigmaExporter.hpp"

namespace vectma {

std::string FigmaExporter::exportToJSON(const SceneGraph& scene) {
    (void)scene;
    // Mock Figma JSON generation logic
    return "{ \"document\": { \"id\": \"0:0\", \"name\": \"Document\", \"type\": \"DOCUMENT\" } }";
}

} // namespace vectma
