#include "ui/Toolbar.hpp"
#include "core/LocaleManager.hpp"
#include <imgui.h>

namespace vectma {

void Toolbar::render(WorkspaceStage& stage) {
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

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
    ImGui::SameLine();
    renderButton("Text", ToolType::Text);
    ImGui::SameLine();
    renderButton("Image (I)", ToolType::Image);
    ImGui::SameLine();
    renderButton("Brush (B)", ToolType::Brush);
    ImGui::SameLine();
    renderButton("Pen (P)", ToolType::Pen);
    ImGui::SameLine();
    renderButton("Artboard (A)", ToolType::Artboard);

    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();

    if (stage.getTool() == ToolType::Brush) {
        float size = stage.getBrushSize();
        if (ImGui::SliderFloat("Size", &size, 1, 100)) stage.setBrushSize(size);
        ImGui::SameLine();
        float bleeding = stage.getBrushBleeding();
        if (ImGui::SliderFloat("Bleeding", &bleeding, 0, 1)) stage.setBrushBleeding(bleeding);
        ImGui::SameLine();
    }

    bool pathEdit = (stage.getEditingMode() == CanvasEditingMode::PathEdit);
    if (pathEdit) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);
    if (ImGui::Button("Path Edit")) {
        stage.setEditingMode(pathEdit ? CanvasEditingMode::Normal : CanvasEditingMode::PathEdit);
    }
    if (pathEdit) ImGui::PopStyleColor();

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

} // namespace vectma
