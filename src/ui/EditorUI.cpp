#include "ui/EditorUI.hpp"
#include "ui/Theme.hpp"
#include "ui/Toolbar.hpp"
#include "ui/Inspector.hpp"
#include "ui/LayerPanel.hpp"
#include "ui/SplashScreen.hpp"
#include "core/PathNode.hpp"
#include "core/LocaleManager.hpp"
#include <imgui.h>

#ifdef VECTMA_USE_OPENGL
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

namespace vectma {

EditorUI::EditorUI(WorkspaceStage& stage, RenderPipeline& renderer)
    : m_stage(stage), m_renderer(renderer) {
    Theme::applyPremiumDark();
    m_layerPanel = std::make_unique<LayerPanel>(m_stage);
    m_showSplash = true;
}

void EditorUI::render() {
#ifdef VECTMA_USE_OPENGL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();

    if (m_showSplash) {
        SplashScreen::render("logo.png");
        m_splashTimer -= 0.016f; // Mock 60fps frame time
        if (m_splashTimer <= 0.0f) m_showSplash = false;
    } else {
        handleInputs();

        // High-Fidelity Stitch Layout Integration: Sidebar Docking
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float sidebarWidth = 250.0f;
        float toolbarHeight = 50.0f;
        float statusBarHeight = 30.0f;

        // Sidebar Left: Layer Panel
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + toolbarHeight));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, viewport->Size.y - toolbarHeight - statusBarHeight));
        m_layerPanel->render();

        // Sidebar Right: Inspector
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x - sidebarWidth, viewport->Pos.y + toolbarHeight));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, viewport->Size.y - toolbarHeight - statusBarHeight));
        Inspector::render(m_stage);

        // Toolbar
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarHeight));
        Toolbar::render(m_stage);

        // Status Bar
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - statusBarHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, statusBarHeight));
        renderStatusBar();

        // Central Viewport
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + sidebarWidth, viewport->Pos.y + toolbarHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 2 * sidebarWidth, viewport->Size.y - toolbarHeight - statusBarHeight));
        renderViewport();

        renderSettingsMenu();
    }

    ImGui::Render();
}

void EditorUI::handleInputs() {
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse) {
        Point2D mousePos(io.MousePos.x, io.MousePos.y);

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            m_stage.handleMouseDown(mousePos);
        } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_stage.handleMouseUp();
        }

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            m_stage.handleMouseMove(mousePos);
        }
    }
}

void EditorUI::renderViewport() {
    ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav);

    m_renderer.beginFrame();
    if (m_stage.getScene()) {
        m_stage.getScene()->render(m_renderer);
    }

    // Render Sub-Selection Overlays
    if (m_stage.isSubSelectionMode()) {
        for (auto node : m_stage.getSelection()) {
            PathNode* path = dynamic_cast<PathNode*>(node);
            if (path) {
                int activeIdx = m_stage.getActiveAnchorIndex();
                int activeHandle = m_stage.getActiveHandleId();
                for (size_t i = 0; i < path->getAnchors().size(); ++i) {
                    m_renderer.drawAnchorOverlay(path->getAnchors()[i], (int)i == activeIdx, ((int)i == activeIdx) ? activeHandle : -1);
                }
            }
        }
    }

    // Render Snapping Guides
    auto snap = m_stage.getActiveSnap();
    if (snap) {
        for (size_t i = 0; i + 1 < snap->guideLines.size(); i += 2) {
            m_renderer.drawSnappingGuide(snap->guideLines[i], snap->guideLines[i+1]);
        }
    }

    m_renderer.endFrame();
    ImGui::End();
}

void EditorUI::renderSettingsMenu() {
    ImGui::Begin("System Settings");

    const char* locales[] = { "English (US)", "Français (FR)" };
    int currentLocale = (int)LocaleManager::getInstance().getLocale();

    if (ImGui::Combo("Language", &currentLocale, locales, 2)) {
        LocaleManager::getInstance().setLocale((Locale)currentLocale);
    }

    ImGui::End();
}

void EditorUI::renderStatusBar() {
    ImGui::Begin("Status Bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);

    Point2D cursor = m_stage.getCursorCanvasPos();
    double scale = m_stage.getScale() * 100.0;
    size_t layerCount = m_stage.getSceneNodeCount();
    size_t selectionCount = m_stage.getSelection().size();

    ImGui::Text("X: %.1f Y: %.1f | Zoom: %.0f%% | Layers: %zu | Selection: %zu",
                cursor.x, cursor.y, scale, layerCount, selectionCount);

    ImGui::End();
}

} // namespace vectma
