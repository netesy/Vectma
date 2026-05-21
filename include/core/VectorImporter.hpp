#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/SceneGraph.hpp"

namespace vectma {

/**
 * @brief Translates third-party vector formats (SVG, PDF) into editable scene graphs.
 */
class VectorImporter {
public:
    /**
     * @brief Parses an SVG string into a scene graph.
     */
    static std::unique_ptr<SceneGraph> ImportSVG(const std::string& svgContent);

    /**
     * @brief Parses a PDF file into a scene graph using PDFium.
     */
    static std::unique_ptr<SceneGraph> ImportPDF(const std::string& filePath);
};

} // namespace vectma
