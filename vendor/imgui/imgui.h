#pragma once
#include <vector>

namespace ImGui {
    struct ImVec2 { float x, y; ImVec2(float _x=0, float _y=0):x(_x),y(_y){} };
    struct ImVec4 { float x, y, z, w; ImVec4(float _x=0, float _y=0, float _z=0, float _w=0):x(_x),y(_y),z(_z),w(_w){} };
    struct ImGuiIO { ImVec2 MousePos; bool WantCaptureMouse=false; };
    ImGuiIO& GetIO();
    void NewFrame();
    void Render();
    struct ImDrawData {};
    ImDrawData* GetDrawData();
    void CreateContext();
    void DestroyContext();
    bool Begin(const char*, bool* = nullptr, int = 0);
    void End();
    void Text(const char*, ...);
    void TextDisabled(const char*, ...);
    void Separator();
    bool Checkbox(const char*, bool*);
    bool Button(const char*, const ImVec2& = ImVec2(0,0));
    void SameLine();
    bool IsMouseClicked(int);
    bool IsMouseReleased(int);
    bool IsMouseDragging(int, float = -1.0f);
    void PushStyleColor(int, unsigned int);
    void PushStyleColor(int, const ImVec4&);
    void PopStyleColor(int = 1);
    struct ImGuiStyle {
        ImVec4 Colors[100];
        float WindowRounding, ChildRounding, FrameRounding, PopupRounding, ScrollbarRounding, GrabRounding;
        float WindowBorderSize, ChildBorderSize, FrameBorderSize, PopupBorderSize, TabBorderSize;
        ImVec2 WindowPadding, FramePadding, ItemSpacing;
    };
    ImGuiStyle& GetStyle();
    struct ImGuiViewport { ImVec2 Pos, Size; };
    ImGuiViewport* GetMainViewport();
    void SetNextWindowPos(const ImVec2&, int = 0, const ImVec2& = ImVec2(0,0));
    void SetNextWindowBgAlpha(float);
    void SetItemDefaultFocus();
    void PushID(int);
    void PopID();
    bool BeginCombo(const char*, const char*, int = 0);
    void EndCombo();
    bool Selectable(const char*, bool = false, int = 0, const ImVec2& = ImVec2(0,0));
    bool SliderFloat(const char*, float*, float, float, const char* = "%.3f", int = 0);
    bool ColorEdit4(const char*, float*, int = 0);
}

typedef ImGui::ImVec2 ImVec2;
typedef ImGui::ImVec4 ImVec4;
typedef ImGui::ImGuiIO ImGuiIO;
typedef ImGui::ImGuiStyle ImGuiStyle;
typedef ImGui::ImGuiViewport ImGuiViewport;
typedef int ImGuiWindowFlags;

#define IMGUI_CHECKVERSION()
enum ImGuiMouseButton_ { ImGuiMouseButton_Left = 0 };
enum ImGuiWindowFlags_ { ImGuiWindowFlags_NoBackground=1, ImGuiWindowFlags_NoDecoration=2, ImGuiWindowFlags_NoInputs=4, ImGuiWindowFlags_NoNav=8, ImGuiWindowFlags_AlwaysAutoResize=16, ImGuiWindowFlags_NoMove=32, ImGuiWindowFlags_NoSavedSettings=64, ImGuiWindowFlags_NoFocusOnAppearing=128 };
enum ImGuiCond_ { ImGuiCond_FirstUseEver=1, ImGuiCond_Always=2 };
enum ImGuiCol_ {
    ImGuiCol_Button=0, ImGuiCol_ButtonActive=1, ImGuiCol_WindowBg=2, ImGuiCol_ChildBg=3,
    ImGuiCol_FrameBg=4, ImGuiCol_FrameBgHovered=5, ImGuiCol_FrameBgActive=6,
    ImGuiCol_Header=7, ImGuiCol_HeaderHovered=8, ImGuiCol_HeaderActive=9,
    ImGuiCol_ButtonHovered=10, ImGuiCol_CheckMark=11, ImGuiCol_SliderGrab=12, ImGuiCol_SliderGrabActive=13
};
