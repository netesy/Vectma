#include "ui/Inspector.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/LocaleManager.hpp"
#include "core/SymbolRegistry.hpp"
#include <imgui.h>

namespace vectma {

void renderPropertyScrubber(const char* label, float& value, float min, float max) {
    ImGui::Text("%s", label);
    ImGui::SameLine();
    ImGui::SliderFloat((std::string("##") + label).c_str(), &value, min, max, "%.1f");
}

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();
    if (selection.empty()) {
        ImGui::TextDisabled("No Selection");
        ImGui::End();
        return;
    }

    CanvasNode* node = selection[0];

    if (ImGui::CollapsingHeader("GEOMETRY", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (RectNode* rect = dynamic_cast<RectNode*>(node)) {
            float x = (float)rect->getX();
            float y = (float)rect->getY();
            float w = (float)rect->getW();
            float h = (float)rect->getH();

            if (ImGui::SliderFloat("X Position", &x, -1000, 5000)) rect->setX(x);
            if (ImGui::SliderFloat("Y Position", &y, -1000, 5000)) rect->setY(y);
            if (ImGui::SliderFloat("Width", &w, 1, 5000)) rect->setW(w);
            if (ImGui::SliderFloat("Height", &h, 1, 5000)) rect->setH(h);
        }
    }

    if (ImGui::CollapsingHeader("APPEARANCE", ImGuiTreeNodeFlags_DefaultOpen)) {
        float opacity = node->getOpacity();
        if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) {
            node->setOpacity(opacity);
        }

        GColor fill = node->getFillColor();
        float col[4] = { (float)fill.r / 255.0f, (float)fill.g / 255.0f, (float)fill.b / 255.0f, (float)fill.a / 255.0f };
        if (ImGui::ColorEdit4("Fill Color", col)) {
            node->setFillColor(GColor((uint8_t)(col[0] * 255.0f), (uint8_t)(col[1] * 255.0f), (uint8_t)(col[2] * 255.0f), (uint8_t)(col[3] * 255.0f)));
        }
    }

    ImGui::End();
}

} // namespace vectma
