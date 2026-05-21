#include "ui/SplashScreen.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/PathData.hpp"
#include <imgui.h>
#include <vector>

namespace vectma {

void SplashScreen::render(const std::string& logoPath) {
    (void)logoPath;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground;

    if (ImGui::Begin("Splash Screen", nullptr, flags)) {
        // High-fidelity procedurally driven vector layout
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(viewport->Pos.x + viewport->Size.x * 0.5f, viewport->Pos.y + viewport->Size.y * 0.5f);

        // Background
        drawList->AddRectFilled(viewport->Pos, ImVec2(viewport->Pos.x + viewport->Size.x, viewport->Pos.y + viewport->Size.y),
                                ImGui::ColorConvertFloat4ToU32(ImVec4(0.075f, 0.075f, 0.075f, 1.0f)));

        // Procedural 'V' logo
        float size = 100.0f;
        ImVec2 p1 = ImVec2(center.x - size * 0.5f, center.y - size * 0.5f);
        ImVec2 p2 = ImVec2(center.x, center.y + size * 0.5f);
        ImVec2 p3 = ImVec2(center.x + size * 0.5f, center.y - size * 0.5f);

        uint32_t primaryCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.867f, 0.718f, 1.0f, 1.0f)); // #ddb7ff
        uint32_t secondaryCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.365f, 0.902f, 1.0f, 1.0f)); // #5de6ff

        drawList->AddLine(p1, p2, primaryCol, 8.0f);
        drawList->AddLine(p2, p3, primaryCol, 8.0f);

        // Anchor points
        drawList->AddCircleFilled(p1, 5.0f, secondaryCol);
        drawList->AddCircleFilled(p2, 5.0f, secondaryCol);
        drawList->AddCircleFilled(p3, 5.0f, secondaryCol);

        // Initializing Workspace text
        const char* text = "Initializing Workspace";
        ImVec2 textSize = ImGui::CalcTextSize(text);
        drawList->AddText(ImVec2(center.x - textSize.x * 0.5f, center.y + size * 1.0f),
                          ImGui::ColorConvertFloat4ToU32(ImVec4(0.9f, 0.9f, 0.9f, 1.0f)), text);

        // Progress bar
        float barWidth = 200.0f;
        float barHeight = 4.0f;
        ImVec2 barPos = ImVec2(center.x - barWidth * 0.5f, center.y + size * 1.5f);
        drawList->AddRectFilled(barPos, ImVec2(barPos.x + barWidth, barPos.y + barHeight),
                                ImGui::ColorConvertFloat4ToU32(ImVec4(0.2f, 0.2f, 0.2f, 1.0f)));
        drawList->AddRectFilled(barPos, ImVec2(barPos.x + barWidth * 0.7f, barPos.y + barHeight), primaryCol);

        ImGui::End();
    }
}

} // namespace vectma
