#include "ui/Toolbar.hpp"
#include <imgui.h>

namespace vectma {

void Toolbar::render(WorkspaceStage& stage) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + 20.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.35f);

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    if (ImGui::Begin("FloatingToolbar", nullptr, window_flags)) {
        auto renderButton = [&](const char* label, ToolType type) {
            bool active = (stage.getTool() == type);
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

            if (ImGui::Button(label)) {
                stage.setTool(type);
            }

            if (active) ImGui::PopStyleColor();
        };

        renderButton("Select", ToolType::Select);
        ImGui::SameLine();
        renderButton("Marquee", ToolType::Marquee);
        ImGui::SameLine();
        renderButton("Rect", ToolType::Rect);
        ImGui::SameLine();
        renderButton("Ellipse", ToolType::Ellipse);
        ImGui::SameLine();
        renderButton("Path", ToolType::Path);

        ImGui::End();
    }
}

} // namespace vectma
