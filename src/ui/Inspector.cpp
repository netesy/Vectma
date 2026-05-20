#include "ui/Inspector.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include <imgui.h>
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

        // Stroke Alignment
        const char* alignments[] = { "Center", "Inside", "Outside" };
        int currentAlign = (int)node->getStrokeAlignment();
        if (ImGui::BeginCombo("Stroke Alignment", alignments[currentAlign])) {
            for (int i = 0; i < 3; i++) {
                bool isSelected = (currentAlign == i);
                if (ImGui::Selectable(alignments[i], isSelected)) {
                    node->setStrokeAlignment((StrokeAlignment)i);
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        // Fill Type
        const char* fillTypes[] = { "Solid", "Linear Gradient", "Radial Gradient" };
        int currentFill = (int)node->getFillType();
        if (ImGui::BeginCombo("Fill Type", fillTypes[currentFill])) {
            for (int i = 0; i < 3; i++) {
                bool isSelected = (currentFill == i);
                if (ImGui::Selectable(fillTypes[i], isSelected)) {
                    node->setFillType((FillType)i);
                }
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (node->getFillType() == FillType::Solid) {
            float color[4] = {
                node->getFillColor().r / 255.0f,
                node->getFillColor().g / 255.0f,
                node->getFillColor().b / 255.0f,
                node->getFillColor().a / 255.0f
            };
            if (ImGui::ColorEdit4("Color", color)) {
                node->setFillColor(GColor(
                    (uint8_t)(color[0] * 255),
                    (uint8_t)(color[1] * 255),
                    (uint8_t)(color[2] * 255),
                    (uint8_t)(color[3] * 255)
                ));
            }
        } else {
            // Gradient Editor
            auto config = node->getGradientConfig();
            bool changed = false;

            ImGui::Text("Gradient Stops");
            for (size_t i = 0; i < config.stops.size(); ++i) {
                ImGui::PushID((int)i);
                float offset = config.stops[i].offset;
                if (ImGui::SliderFloat("Offset", &offset, 0.0f, 1.0f)) {
                    config.stops[i].offset = offset;
                    changed = true;
                }

                // Color as float array
                uint32_t c = config.stops[i].color;
                float color[4] = {
                    ((c >> 24) & 0xFF) / 255.0f,
                    ((c >> 16) & 0xFF) / 255.0f,
                    ((c >> 8) & 0xFF) / 255.0f,
                    (c & 0xFF) / 255.0f
                };
                if (ImGui::ColorEdit4("Stop Color", color)) {
                    uint32_t newColor =
                        ((uint32_t)(color[0] * 255) << 24) |
                        ((uint32_t)(color[1] * 255) << 16) |
                        ((uint32_t)(color[2] * 255) << 8) |
                        ((uint32_t)(color[3] * 255));
                    config.stops[i].color = newColor;
                    changed = true;
                }
                ImGui::PopID();
            }

            if (ImGui::Button("Add Stop")) {
                config.stops.push_back({1.0f, 0x000000FF});
                changed = true;
            }

            if (changed) {
                node->setGradientConfig(config);
            }
        }
    }

    ImGui::End();
}

} // namespace vectma
