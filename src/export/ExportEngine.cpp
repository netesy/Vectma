#include "export/ExportEngine.hpp"
#include "core/ArtboardNode.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include <sstream>
namespace vectma { namespace export_engine {
std::string ExportEngine::exportToSVG(CanvasNode* root) {
    if (!root) return "";
    std::stringstream ss; GRect bbox = root->computeBoundingBox();
    ss << "<svg width=\"" << bbox.width << "\" height=\"" << bbox.height << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    for (const auto& child : root->getChildren()) ss << child->toSVG() << "\n";
    ss << "</svg>"; return ss.str();
}
std::vector<uint8_t> ExportEngine::exportToPDF(CanvasNode*) { return {}; }
std::string ExportEngine::generateAUI(CanvasNode* node) {
    if (!node) return "";
    return "std::make_shared<aui::AView>()->setStyle(\"x: " + std::to_string(node->getX()) + "\")";
}
}}
