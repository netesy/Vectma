#include "ui/Inspector.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/PDFExporter.hpp"
#include "core/FigmaExporter.hpp"
#include <imgui.h>
#include <cstring>
#include <string>

namespace vectma {

void renderExportDashboard(WorkspaceStage& stage) {
    if (ImGui::CollapsingHeader("Export Canvas", ImGuiTreeNodeFlags_DefaultOpen)) {
        static int formatIdx = 0;
        const char* formats[] = { "PNG", "SVG", "PDF", "Figma JSON" };
        ImGui::Combo("Format", &formatIdx, formats, 4);

        static float scale = 1.0f;
        ImGui::SliderFloat("Scale", &scale, 1.0f, 4.0f, "%.1fx");

        if (ImGui::Button("Export Now")) {
            auto scene = stage.getScene();
            if (scene) {
                if (formatIdx == 0) { // PNG
                } else if (formatIdx == 1) { // SVG
                    std::string svg = scene->toSVG();
                } else if (formatIdx == 2) { // PDF
                    std::string pdf = PDFExporter::exportToPDF(*scene);
                } else if (formatIdx == 3) { // Figma
                    std::string json = FigmaExporter::exportToJSON(*scene);
                }
            }
        }
    }
}

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();
    if (selection.empty()) {
        ImGui::TextDisabled("No objects selected");
        ImGui::Separator();
        renderExportDashboard(stage);
        ImGui::End();
        return;
    }

    if (selection.size() > 1) {
        ImGui::Text("%d objects selected", (int)selection.size());
    } else {
        CanvasNode* node = selection[0];
        ImGui::Text("Type: %s", node->getClassName().c_str());
        ImGui::Separator();

        bool visible = node->isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            node->setVisibility(visible);
        }

        const char* alignments[] = { "Center", "Inside", "Outside" };
        int currentAlign = (int)node->getStrokeAlignment();
        if (ImGui::BeginCombo("Stroke Alignment", alignments[currentAlign])) {
            for (int i = 0; i < 3; i++) {
                if (ImGui::Selectable(alignments[i], currentAlign == i)) {
                    node->setStrokeAlignment((StrokeAlignment)i);
                }
            }
            ImGui::EndCombo();
        }

        if (TextNode* textNode = dynamic_cast<TextNode*>(node)) {
            ImGui::Separator();
            ImGui::Text("Typography");

            float fontSize = textNode->font_size;
            if (ImGui::SliderFloat("Font Size", &fontSize, 6.0f, 120.0f)) {
                textNode->font_size = fontSize;
            }

            float tracking = textNode->tracking;
            if (ImGui::SliderFloat("Tracking", &tracking, -5.0f, 20.0f)) {
                textNode->tracking = tracking;
            }

            float leading = textNode->leading;
            if (ImGui::SliderFloat("Leading", &leading, 0.5f, 3.0f)) {
                textNode->leading = leading;
            }

            char buf[256];
            std::strncpy(buf, textNode->text_buffer.c_str(), sizeof(buf));
            buf[sizeof(buf)-1] = '\0';
            if (ImGui::InputTextMultiline("Text", buf, sizeof(buf))) {
                textNode->text_buffer = buf;
            }
        }
    }

    ImGui::Separator();
    renderExportDashboard(stage);

    ImGui::End();
}

} // namespace vectma
