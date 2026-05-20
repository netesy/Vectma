#pragma once

#include "core/SceneGraph.hpp"
#include <string>

namespace vectma {

class FigmaExporter {
public:
    static std::string exportToJSON(const SceneGraph& scene);
};

} // namespace vectma
