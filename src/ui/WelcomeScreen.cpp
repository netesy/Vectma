#include "ui/WelcomeScreen.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/FileEngine.hpp"
#include "core/SceneGraph.hpp"
#include <imgui.h>
#include <iostream>

namespace vectma {

void WelcomeScreen::render(WorkspaceStage& stage, bool& showWelcome) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.075f, 0.075f, 0.075f, 1.0f));
    if (ImGui::Begin("Welcome Screen", nullptr, flags)) {

        ImGui::Text("VECTMA STUDIO");
        ImGui::Separator();

        ImGui::Text("Welcome to the future of vector design.");
        ImGui::NewLine();

        if (ImGui::Button("New Project", ImVec2(200, 40))) {
            stage.setScene(std::make_shared<SceneGraph>());
            showWelcome = false;
        }

        if (ImGui::Button("Open from Disk", ImVec2(200, 40))) {
            FileEngine::ImportFromVectma("project.vectma");
            showWelcome = false;
        }

        ImGui::NewLine();
        ImGui::Text("Recent Files");
        ImGui::Separator();
        if (ImGui::Selectable("Branding_Guidelines.vectma")) { showWelcome = false; }
        if (ImGui::Selectable("UI_Kit_v2.vectma")) { showWelcome = false; }

        ImGui::End();
    }
    ImGui::PopStyleColor();
}

} // namespace vectma
