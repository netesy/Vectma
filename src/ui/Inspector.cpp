#include "ui/Inspector.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include <imgui.h>
#include <cstring>
#include <string>

namespace vectma {

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();
    if (selection.empty()) {
        ImGui::TextDisabled("No objects selected");
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

        // Common stroke/fill settings
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

        // Type-specific properties
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

    ImGui::End();
}

} // namespace vectma
