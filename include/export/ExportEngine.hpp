#pragma once
#include "core/SceneGraph.hpp"
#include "prototype/InteractionGraph.hpp"
#include <string>
#include <vector>
namespace vectma { namespace export_engine {
class ExportEngine {
public:
    static std::string exportToSVG(CanvasNode* root);
    static std::vector<uint8_t> exportToPDF(CanvasNode* root);
    static std::string generateAUI(CanvasNode* node);
};
}}
