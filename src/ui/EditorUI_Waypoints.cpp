#include "ui/EditorUI.hpp"
#include "core/ArtboardNode.hpp"
#include "core/Commands.hpp"
#include <imgui.h>

namespace vectma {

void EditorUI::renderWaypointHUD() {
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::Begin("Waypoints", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    auto& wm = m_stage.getWaypointManager();
    auto viewport = ImGui::GetMainViewport();

    for (int i = 0; i < WaypointManager::MAX_WAYPOINTS; ++i) {
        auto wp = wm.getWaypoint(i);
        char buf[32];
        sprintf(buf, "Slot %d", i + 1);

        if (wp) {
            if (ImGui::Button(wp->name.c_str())) {
                m_stage.jumpToWaypoint(i);
            }
            ImGui::SameLine();
            if (ImGui::Button(("X##" + std::to_string(i)).c_str())) {
                wm.clearWaypoint(i);
            }
        } else {
            if (ImGui::Button(("+##" + std::to_string(i)).c_str())) {
                // Save current view as waypoint
                auto center = m_stage.screenToCanvas(Point2D(viewport->Size.x/2, viewport->Size.y/2));
                wm.saveWaypoint(i, "View " + std::to_string(i + 1), center.x, center.y, m_stage.getScale());
            }
        }
        if ((i + 1) % 3 != 0) ImGui::SameLine();
    }
    ImGui::End();
}

void EditorUI::renderArtboardPresets() {
    if (m_stage.getTool() != ToolType::Artboard) return;

    auto viewport = ImGui::GetMainViewport();

    ImGui::Begin("Artboard Presets");
    for (const auto& preset : ArtboardNode::GetPresets()) {
        if (ImGui::Button(preset.name.c_str())) {
            auto center = m_stage.screenToCanvas(Point2D(viewport->Size.x/2, viewport->Size.y/2));
            auto ab = std::make_unique<ArtboardNode>(preset.name, GRect(center.x - preset.width/2.0, center.y - preset.height/2.0, preset.width, preset.height));
            m_stage.executeCommand(std::make_unique<AddNodeCommand>(m_stage.getScene().get(), std::move(ab)));
        }
    }
    ImGui::End();
}

} // namespace vectma
