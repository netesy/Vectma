#include "ui/Theme.hpp"
#include <imgui.h>

namespace vectma {

void Theme::applyPremiumDark() {
    auto& style = ImGui::GetStyle();

    // Backgrounds
    style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.043f, 0.043f, 0.047f, 1.0f); // #0B0B0C
    style.Colors[ImGuiCol_ChildBg]          = ImVec4(0.071f, 0.071f, 0.078f, 1.0f); // #121214
    style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.071f, 0.071f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.091f, 0.091f, 0.098f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive]    = ImVec4(0.111f, 0.111f, 0.118f, 1.0f);

    // Teal Highlights
    ImVec4 teal = ImVec4(0.0f, 0.5f, 0.5f, 1.0f);
    style.Colors[ImGuiCol_Header]           = ImVec4(0.0f, 0.5f, 0.5f, 0.7f);
    style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.0f, 0.5f, 0.5f, 0.8f);
    style.Colors[ImGuiCol_HeaderActive]     = teal;

    style.Colors[ImGuiCol_Button]           = ImVec4(0.071f, 0.071f, 0.078f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.0f, 0.5f, 0.5f, 0.6f);
    style.Colors[ImGuiCol_ButtonActive]     = teal;

    style.Colors[ImGuiCol_CheckMark]        = teal;
    style.Colors[ImGuiCol_SliderGrab]       = teal;
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.6f, 0.6f, 1.0f);

    // Premium Soft Geometry
    style.WindowRounding = 12.0f;
    style.ChildRounding  = 10.0f;
    style.FrameRounding  = 8.0f;
    style.PopupRounding  = 10.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 6.0f;

    // Eliminate harsh dividers
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize  = 0.0f;
    style.FrameBorderSize  = 0.0f;
    style.PopupBorderSize  = 0.0f;
    style.TabBorderSize    = 0.0f;

    // Spacious padding
    style.WindowPadding    = ImVec2(15, 15);
    style.FramePadding     = ImVec2(8, 6);
    style.ItemSpacing      = ImVec2(10, 8);
}

} // namespace vectma
