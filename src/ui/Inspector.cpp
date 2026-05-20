#include "ui/Inspector.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/PDFExporter.hpp"
#include "core/FigmaExporter.hpp"
#include "core/LocaleManager.hpp"
#include <imgui.h>
#include <cstring>
#include <string>

namespace vectma {

void renderExportDashboard(WorkspaceStage& stage) {
    if (ImGui::CollapsingHeader(V_TXT("export.canvas"), ImGuiTreeNodeFlags_DefaultOpen)) {
        static int formatIdx = 0;
        const char* formats[] = { "PNG", "SVG", "PDF", "Figma JSON" };
        ImGui::Combo(V_TXT("export.format"), &formatIdx, formats, 4);

        static float scale = 1.0f;
        ImGui::SliderFloat(V_TXT("export.scale"), &scale, 1.0f, 4.0f, "%.1fx");

        if (ImGui::Button(V_TXT("export.now"))) {
            auto scene = stage.getScene();
            if (scene) {
                if (formatIdx == 0) {
                } else if (formatIdx == 1) {
                    std::string svg = scene->toSVG();
                } else if (formatIdx == 2) {
                    std::string pdf = PDFExporter::exportToPDF(*scene);
                } else if (formatIdx == 3) {
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
        ImGui::Text("%s%s", V_TXT("inspector.type"), node->getClassName().c_str());
        ImGui::Separator();

        bool visible = node->isVisible();
        if (ImGui::Checkbox(V_TXT("inspector.visible"), &visible)) {
            node->setVisibility(visible);
        }

        const char* alignments[] = { "Center", "Inside", "Outside" };
        int currentAlign = (int)node->getStrokeAlignment();
        if (ImGui::BeginCombo(V_TXT("inspector.stroke_alignment"), alignments[currentAlign])) {
            for (int i = 0; i < 3; i++) {
                if (ImGui::Selectable(alignments[i], currentAlign == i)) {
                    node->setStrokeAlignment((StrokeAlignment)i);
                }
            }
            ImGui::EndCombo();
        }

        if (PathNode* pathNode = dynamic_cast<PathNode*>(node)) {
            ImGui::Separator();
            ImGui::Text("Path Effects");
            ImGui::SliderFloat("Corner Radius", &pathNode->cornerRadius, 0.0f, 50.0f);
            ImGui::SliderFloat("Stroke Offset", &pathNode->strokeOffset, -20.0f, 20.0f);

            bool isDashed = !pathNode->dashPattern.empty();
            if (ImGui::Checkbox("Dashed Stroke", &isDashed)) {
                if (isDashed) pathNode->dashPattern = { 5.0f, 5.0f };
                else pathNode->dashPattern.clear();
            }
        }

        if (TextNode* textNode = dynamic_cast<TextNode*>(node)) {
            ImGui::Separator();
            ImGui::Text(V_TXT("inspector.typography"));

            float fontSize = textNode->font_size;
            if (ImGui::SliderFloat(V_TXT("inspector.font_size"), &fontSize, 6.0f, 120.0f)) {
                textNode->font_size = fontSize;
            }

            float tracking = textNode->tracking;
            if (ImGui::SliderFloat(V_TXT("inspector.tracking"), &tracking, -5.0f, 20.0f)) {
                textNode->tracking = tracking;
            }

            float leading = textNode->leading;
            if (ImGui::SliderFloat(V_TXT("inspector.leading"), &leading, 0.5f, 3.0f)) {
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
