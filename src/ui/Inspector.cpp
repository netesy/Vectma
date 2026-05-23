#include "ui/Inspector.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/MasterComponentNode.hpp"
#include "core/ComponentInstanceNode.hpp"
#include "core/SymbolRegistry.hpp"
#include "core/LocaleManager.hpp"
#include <imgui.h>
#include <vector>
#include <string>
#include <cstring>

namespace vectma {

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();

    // TASK 3: Component Actions
    if (ImGui::CollapsingHeader("COMPONENTS", 1)) {
        if (!selection.empty()) {
            if (ImGui::Button("Create Component")) stage.createComponentFromSelection();

            if (selection.size() == 1) {
                if (auto* inst = dynamic_cast<ComponentInstanceNode*>(selection[0])) {
                    if (ImGui::Button("Detach Instance")) stage.detachInstance(inst);
                }
            }
        }

        ImGui::Separator();
        ImGui::Text("Asset Library");
        auto symbols = SymbolRegistry::getInstance().getAllSymbolIDs();
        for (const auto& id : symbols) {
            if (ImGui::Selectable(id.c_str())) {
                stage.placeInstance(id);
            }
        }
    }

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
            if (ImGui::SliderFloat("Size", &textNode->font_size, 4, 256)) textNode->setFontSize(textNode->font_size);
        }
    }

    if (ImGui::CollapsingHeader("APPEARANCE", 1)) {
        float opacity = node->getOpacity();
        if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) node->setOpacity(opacity);

        GColor fill = node->getFillColor();
        float col[4] = { (float)fill.r / 255.0f, (float)fill.g / 255.0f, (float)fill.b / 255.0f, (float)fill.a / 255.0f };
        [[maybe_unused]] float* pCol = col;
    }

    ImGui::End();
}

} // namespace vectma
