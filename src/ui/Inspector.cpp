#include "ui/Inspector.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/LocaleManager.hpp"
#include "core/SymbolRegistry.hpp"
#include <imgui.h>
#include <vector>
#include <string>
#include <cstring>

namespace vectma {

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();
    if (selection.empty()) {
        ImGui::TextDisabled("No Selection");
        ImGui::End();
        return;
    }

    if (selection.size() > 1) {
        if (ImGui::CollapsingHeader("PATH FINDER", 1)) {
            if (ImGui::Button("Union"))    stage.applyBooleanOperation(BooleanOp::Union);
            ImGui::SameLine();
            if (ImGui::Button("Subtract")) stage.applyBooleanOperation(BooleanOp::Subtract);
            ImGui::SameLine();
            if (ImGui::Button("Intersect")) stage.applyBooleanOperation(BooleanOp::Intersect);
            ImGui::SameLine();
            if (ImGui::Button("Exclude"))   stage.applyBooleanOperation(BooleanOp::Exclude);
        }
    }

    CanvasNode* node = selection[0];

    if (ImGui::CollapsingHeader("GEOMETRY", 1)) {
        if (RectNode* rect = dynamic_cast<RectNode*>(node)) {
            float x = (float)rect->getX();
            float y = (float)rect->getY();
            float w = (float)rect->getW();
            float h = (float)rect->getH();
            if (ImGui::SliderFloat("X", &x, -5000, 5000)) rect->setX(x);
            if (ImGui::SliderFloat("Y", &y, -5000, 5000)) rect->setY(y);
            if (ImGui::SliderFloat("W", &w, 1, 5000)) rect->setW(w);
            if (ImGui::SliderFloat("H", &h, 1, 5000)) rect->setH(h);
        }
    }

    if (TextNode* textNode = dynamic_cast<TextNode*>(node)) {
        if (ImGui::CollapsingHeader("TYPOGRAPHY", 1)) {
            char buf[1024];
            std::strncpy(buf, textNode->getText().c_str(), sizeof(buf));
            // if (ImGui::InputTextMultiline("Text", buf, sizeof(buf))) textNode->setText(buf);

            float fsize = textNode->getFontSize();
            if (ImGui::SliderFloat("Size", &fsize, 4, 256)) textNode->setFontSize(fsize);
        }
    }

    if (ImGui::CollapsingHeader("APPEARANCE", 1)) {
        float opacity = node->getOpacity();
        if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) node->setOpacity(opacity);

        GColor fill = node->getFillColor();
        [[maybe_unused]] float col[4] = { (float)fill.r / 255.0f, (float)fill.g / 255.0f, (float)fill.b / 255.0f, (float)fill.a / 255.0f };
        // if (ImGui::ColorEdit4("Color", col)) {
        //     node->setFillColor(GColor((uint8_t)(col[0] * 255.0f), (uint8_t)(col[1] * 255.0f), (uint8_t)(col[2] * 255.0f), (uint8_t)(col[3] * 255.0f)));
        // }
    }

    ImGui::End();
}

} // namespace vectma
