#include "ui/Toolbar.hpp"
#include "core/LocaleManager.hpp"
#include <imgui.h>

namespace vectma {

void Toolbar::render(WorkspaceStage& stage) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + 20.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("FloatingToolbar", nullptr, window_flags)) {
        auto renderButton = [&](const char* key, ToolType type) {
            bool active = (stage.getTool() == type);
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

            if (ImGui::Button(V_TXT(key))) {
                stage.setTool(type);
            }

            if (active) ImGui::PopStyleColor();
        };

        renderButton("toolbar.select", ToolType::Select);
        ImGui::SameLine();
        renderButton("toolbar.marquee", ToolType::Marquee);
        ImGui::SameLine();
        renderButton("toolbar.rect", ToolType::Rect);
        ImGui::SameLine();
        renderButton("toolbar.ellipse", ToolType::Ellipse);
        ImGui::SameLine();
        renderButton("toolbar.path", ToolType::Path);
        ImGui::SameLine();
        renderButton("toolbar.text", ToolType::Text);

        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        bool subSel = stage.isSubSelectionMode();
        if (subSel) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
        if (ImGui::Button(V_TXT("toolbar.subselection"))) {
            stage.setSubSelectionMode(!subSel);
        }
        if (subSel) ImGui::PopStyleColor();

        // Undo / Redo
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        if (!stage.canUndo()) ImGui::BeginDisabled();
        if (ImGui::Button("Undo")) stage.undo();
        if (!stage.canUndo()) ImGui::EndDisabled();

        ImGui::SameLine();

        if (!stage.canRedo()) ImGui::BeginDisabled();
        if (ImGui::Button("Redo")) stage.redo();
        if (!stage.canRedo()) ImGui::EndDisabled();

        ImGui::End();
    }
}

} // namespace vectma
