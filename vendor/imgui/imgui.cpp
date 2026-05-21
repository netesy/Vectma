#include "imgui.h"
namespace ImGui {
    ImGuiIO io;
    ImGuiIO& GetIO() { return io; }
    void NewFrame() {}
    void Render() {}
    ImDrawData* GetDrawData() { return nullptr; }
    void CreateContext() {}
    void DestroyContext() {}
    bool Begin(const char*, bool*, int) { return true; }
    void End() {}
    void Text(const char*, ...) {}
    void TextDisabled(const char*, ...) {}
    void Separator() {}
    void NewLine() {}
    bool Checkbox(const char*, bool*) { return false; }
    bool Button(const char*, const ImVec2&) { return false; }
    void SameLine() {}
    bool IsMouseClicked(int) { return false; }
    bool IsMouseReleased(int) { return false; }
    bool IsMouseDragging(int, float) { return false; }
    void PushStyleColor(int, unsigned int) {}
    void PushStyleColor(int, const ImVec4&) {}
    void PopStyleColor(int) {}
    ImGuiStyle style;
    ImGuiStyle& GetStyle() { return style; }
    ImGuiViewport viewport;
    ImGuiViewport* GetMainViewport() { return &viewport; }
    void SetNextWindowPos(const ImVec2&, int, const ImVec2&) {}
    void SetNextWindowSize(const ImVec2&, int) {}
    void SetNextWindowBgAlpha(float) {}
    void SetItemDefaultFocus() {}
    void PushID(int) {}
    void PopID() {}
    void BeginDisabled(bool) {} void EndDisabled() {}
    bool BeginCombo(const char*, const char*, int) { return true; }
    bool Combo(const char*, int*, const char* const[], int, int) { return false; } bool CollapsingHeader(const char*, int) { return false; }
    void EndCombo() {}
    bool Selectable(const char*, bool, int, const ImVec2&) { return false; }
    bool SliderFloat(const char*, float*, float, float, const char*, int) { return false; }
    bool ColorEdit4(const char*, float*, int) { return false; }
    bool InputTextMultiline(const char*, char*, size_t, const ImVec2&, int) { return false; }
}
