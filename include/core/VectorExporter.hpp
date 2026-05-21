#pragma once

#include <string>
#include <vector>
#include "core/SceneGraph.hpp"

namespace vectma {

/**
 * @brief Generates resolution-independent vector output (PDF, SVG).
 */
class VectorExporter {
public:
    /**
     * @brief Exports the scene graph to a high-fidelity PDF.
     */
    static bool ExportToPDF(const std::string& filePath, const SceneGraph& scene);

    /**
     * @brief Exports the scene graph to standard SVG markup.
     */
    static bool ExportToSVG(const std::string& filePath, const SceneGraph& scene);
};

} // namespace vectma
