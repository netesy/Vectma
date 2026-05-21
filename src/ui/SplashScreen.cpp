#include "ui/SplashScreen.hpp"
#include <imgui.h>

namespace vectma {

void SplashScreen::render(const std::string& logoPath) {
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
    if (ImGui::Begin("Splash Screen", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs)) {
        ImGui::Text("Vectma Engine Loading...");
        ImGui::Text("Logo: %s", logoPath.c_str());
        // In a real GL app, we would load logoPath into a texture and DrawImage here.
        ImGui::End();
    }
}

} // namespace vectma
