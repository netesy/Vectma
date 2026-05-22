#include "ui/Inspector.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
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

    CanvasNode* node = selection[0];

    // Transformation / Geometry
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
        } else if (TextNode* text = dynamic_cast<TextNode*>(node)) {
            float x = text->getX();
            float y = text->getY();
            if (ImGui::SliderFloat("X", &x, -5000, 5000)) text->setX(x);
            if (ImGui::SliderFloat("Y", &y, -5000, 5000)) text->setY(y);
        }
    }

    // TASK 2: Advanced Typography Controls
    if (TextNode* textNode = dynamic_cast<TextNode*>(node)) {
        if (ImGui::CollapsingHeader("TYPOGRAPHY", 1)) {
            // Text Buffer Input
            char buf[1024];
            std::strncpy(buf, textNode->getText().c_str(), sizeof(buf));
            if (ImGui::InputTextMultiline("Content", buf, sizeof(buf))) {
                textNode->setText(buf);
            }

            // Font Family Dropdown
            const char* fonts[] = { "Inter", "Poppins", "Roboto", "JetBrains Mono" };
            int currentFont = 0;
            for (int i = 0; i < 4; i++) {
                if (textNode->font_family == fonts[i]) {
                    currentFont = i;
                    break;
                }
            }
            if (ImGui::Combo("Family", &currentFont, fonts, 4)) {
                textNode->font_family = fonts[currentFont];
            }

            // Font Size Slider
            float fsize = textNode->getFontSize();
            if (ImGui::SliderFloat("Size (pt)", &fsize, 4, 256)) {
                textNode->setFontSize(fsize);
            }

            // Leading Slider
            float leading = textNode->leading;
            if (ImGui::SliderFloat("Leading", &leading, 0.5f, 3.0f)) {
                textNode->leading = leading;
            }
        }
    }

    // Appearance & TASK 3: Blending Menu
    if (ImGui::CollapsingHeader("APPEARANCE", 1)) {
        float opacity = node->getOpacity();
        if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) {
            node->setOpacity(opacity);
        }

        // Blending Mode Dropdown
        const char* blendModes[] = {
            "Normal", "Multiply", "Screen", "Overlay", "Darken", "Lighten",
            "ColorDodge", "ColorBurn", "HardLight", "SoftLight", "Difference", "Exclusion"
        };
        int currentBlend = (int)node->getBlendMode();
        if (ImGui::Combo("Blending", &currentBlend, blendModes, 12)) {
            node->setBlendMode((BlendMode)currentBlend);
        }

        GColor fill = node->getFillColor();
        float col[4] = { (float)fill.r / 255.0f, (float)fill.g / 255.0f, (float)fill.b / 255.0f, (float)fill.a / 255.0f };
        if (ImGui::SliderFloat("R", &col[0], 0, 1)) {}
        if (ImGui::SliderFloat("G", &col[1], 0, 1)) {}
        if (ImGui::SliderFloat("B", &col[2], 0, 1)) {}
        if (ImGui::SliderFloat("A", &col[3], 0, 1)) {
             node->setFillColor(GColor((uint8_t)(col[0] * 255.0f), (uint8_t)(col[1] * 255.0f), (uint8_t)(col[2] * 255.0f), (uint8_t)(col[3] * 255.0f)));
        }
    }

    ImGui::End();
}

} // namespace vectma
