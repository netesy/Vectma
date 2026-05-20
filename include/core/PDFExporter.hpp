#pragma once

#include "core/SceneGraph.hpp"
#include <string>

namespace vectma {

class PDFExporter {
public:
    static std::string exportToPDF(const SceneGraph& scene);
};

} // namespace vectma
