#include "ui/Inspector.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/InstanceNode.hpp"
#include "core/PDFExporter.hpp"
#include "core/FigmaExporter.hpp"
#include "core/LocaleManager.hpp"
#include "core/SymbolRegistry.hpp"
#include <imgui.h>
#include <fstream>
#include <string>

namespace vectma {

void renderExportDashboard(WorkspaceStage& stage) {
    if (ImGui::CollapsingHeader(V_TXT("export.canvas"), ImGuiTreeNodeFlags_DefaultOpen)) {
        static int formatIdx = 1; // Default to SVG
        const char* formats[] = { "PNG", "SVG", "PDF", "Figma JSON" };
        ImGui::Combo(V_TXT("export.format"), &formatIdx, formats, 4);

        static float scale = 1.0f;
        ImGui::SliderFloat(V_TXT("export.scale"), &scale, 1.0f, 4.0f, "%.1fx");

        if (ImGui::Button(V_TXT("export.now"))) {
            auto scene = stage.getScene();
            if (scene) {
                std::string content;
                std::string ext;
                if (formatIdx == 1) {
                    content = scene->toSVG();
                    ext = ".svg";
                } else if (formatIdx == 2) {
                    content = PDFExporter::exportToPDF(*scene);
                    ext = ".pdf";
                } else if (formatIdx == 3) {
                    content = FigmaExporter::exportToJSON(*scene);
                    ext = ".json";
                }

                if (!content.empty()) {
                    std::ofstream file("exported_design" + ext);
                    file << content;
                }
            }
        }
    }
}

void renderSymbolRegistry(WorkspaceStage& stage) {
    if (ImGui::CollapsingHeader("Asset Palette (Symbols)", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto ids = SymbolRegistry::getInstance().getAllSymbolIDs();
        for (const auto& id : ids) {
            if (ImGui::Button(id.c_str())) {
                auto instance = std::make_unique<InstanceNode>(id);
                if (stage.getScene()) stage.getScene()->addChild(std::move(instance));
            }
            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
}

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();
    if (selection.empty()) {
        ImGui::TextDisabled("No objects selected");
        ImGui::Separator();
        renderExportDashboard(stage);
        ImGui::Separator();
        renderSymbolRegistry(stage);
        ImGui::End();
        return;
    }

    if (selection.size() > 1) {
        ImGui::Text("%d objects selected", (int)selection.size());
        if (ImGui::Button("Create Component")) {
            // Registering new component prototype from active selection
            std::string id = "Component_" + std::to_string(rand() % 1000);
            SymbolRegistry::getInstance().registerSymbol(id, std::make_unique<RectNode>(0, 0, 100, 100));
        }
    } else {
        CanvasNode* node = selection[0];
        ImGui::Text("%s%s", V_TXT("inspector.type"), node->getClassName().c_str());
        ImGui::Separator();

        bool visible = node->isVisible();
        if (ImGui::Checkbox(V_TXT("inspector.visible"), &visible)) {
            node->setVisibility(visible);
        }

        if (TextNode* textNode = dynamic_cast<TextNode*>(node)) {
            ImGui::Separator();
            ImGui::Text(V_TXT("inspector.typography"));
            ImGui::SliderFloat(V_TXT("inspector.font_size"), &textNode->font_size, 6.0f, 120.0f);
        }
    }

    ImGui::Separator();
    renderExportDashboard(stage);
    ImGui::Separator();
    renderSymbolRegistry(stage);

    ImGui::End();
}

} // namespace vectma
