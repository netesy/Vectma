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
#include "style/TokenRegistry.hpp"
#include "core/modifiers/CornerRoundingModifier.hpp"
#include <imgui.h>
#include <vector>
#include <string>
#include <cstring>

namespace vectma {

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();
    auto& sm = stage.getSelectionManager();

    if (ImGui::CollapsingHeader("THEME", ImGuiTreeNodeFlags_DefaultOpen)) {
        ThemeType currentTheme = TokenRegistry::getInstance().getActiveTheme();
        const char* themes[] = { "Light", "Dark" };
        int activeIdx = (currentTheme == ThemeType::Light) ? 0 : 1;
        if (ImGui::Combo("Active Theme", &activeIdx, themes, 2)) {
            TokenRegistry::getInstance().switchTheme(activeIdx == 0 ? ThemeType::Light : ThemeType::Dark);
        }
    }

    if (sm.getCount() >= 3) {
        if (ImGui::CollapsingHeader("DISTRIBUTION", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Button("Distribute Horizontally")) sm.distributeHorizontally();
            ImGui::SameLine();
            if (ImGui::Button("Distribute Vertically")) sm.distributeVertically();
        }
    }

    if (ImGui::CollapsingHeader("COMPONENTS", ImGuiTreeNodeFlags_DefaultOpen)) {
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
        if (ImGui::CollapsingHeader("PATH FINDER", ImGuiTreeNodeFlags_DefaultOpen)) {
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

    if (ImGui::CollapsingHeader("GEOMETRY", ImGuiTreeNodeFlags_DefaultOpen)) {
        float x = (float)node->getX();
        float y = (float)node->getY();
        float w = (float)node->getWidth();
        float h = (float)node->getHeight();

        bool posChanged = false;
        if (ImGui::SliderFloat("X", &x, -5000, 5000)) posChanged = true;
        if (ImGui::SliderFloat("Y", &y, -5000, 5000)) posChanged = true;
        if (posChanged) node->setPositionRemote(x, y, LamportClock::getInstance().tick());

        bool sizeChanged = false;
        if (ImGui::SliderFloat("W", &w, 1, 5000)) sizeChanged = true;
        if (ImGui::SliderFloat("H", &h, 1, 5000)) sizeChanged = true;
        if (sizeChanged) node->setSizeRemote(w, h, LamportClock::getInstance().tick());
    }

    if (TextNode* textNode = dynamic_cast<TextNode*>(node)) {
        if (ImGui::CollapsingHeader("TYPOGRAPHY", ImGuiTreeNodeFlags_DefaultOpen)) {
            float fontSize = textNode->getFontSize();
            if (ImGui::SliderFloat("Size", &fontSize, 4, 256)) textNode->setFontSize(fontSize);

            std::string text = textNode->getText();
            char buf[1024];
            std::strncpy(buf, text.c_str(), sizeof(buf));
            if (ImGui::InputTextMultiline("Content", buf, sizeof(buf))) {
                textNode->setText(std::string(buf));
            }
        }
    }

    if (ImGui::CollapsingHeader("APPEARANCE", ImGuiTreeNodeFlags_DefaultOpen)) {
        float opacity = node->getOpacity();
        if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) node->setOpacity(opacity);

        GColor fill = node->getFillColor();
        float col[4] = { (float)fill.r / 255.0f, (float)fill.g / 255.0f, (float)fill.b / 255.0f, (float)fill.a / 255.0f };
        if (ImGui::ColorEdit4("Fill", col)) {
            node->setFillColor(GColor((uint8_t)(col[0]*255), (uint8_t)(col[1]*255), (uint8_t)(col[2]*255), (uint8_t)(col[3]*255)));
        }

        ImGui::Separator();
        ImGui::Text("Token Binding");
        if (ImGui::Button("Bind color.primary")) node->setFillColorToken("color.primary");
        ImGui::SameLine();
        if (ImGui::Button("Bind color.bg")) node->setFillColorToken("color.bg");

        ImGui::Separator();
        double sw = node->getStrokeWidth();
        float fsw = (float)sw;
        if (ImGui::SliderFloat("Stroke Width", &fsw, 0, 100)) node->setStrokeWidth(fsw);
        if (ImGui::Button("Bind spacing.gutter")) node->setStrokeWidthToken("spacing.gutter");
    }

    if (auto* pathNode = dynamic_cast<PathNode*>(node)) {
        if (ImGui::CollapsingHeader("MODIFIERS", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (auto& modifier : const_cast<std::vector<std::unique_ptr<Modifier>>&>(pathNode->getModifierStack())) {
                if (auto* rounding = dynamic_cast<CornerRoundingModifier*>(modifier.get())) {
                    float r = rounding->getRadius();
                    if (ImGui::SliderFloat("Corner Radius", &r, 0, 100)) rounding->setRadius(r);
                    if (ImGui::Button("Bind radius.card")) rounding->setRadiusToken("radius.card");
                }
            }
        }
    }

    ImGui::End();
}

} // namespace vectma
