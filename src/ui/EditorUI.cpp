#include "ui/EditorUI.hpp"
#include "ui/Theme.hpp"
#include "ui/Toolbar.hpp"
#include "ui/Inspector.hpp"
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
}

void EditorUI::render() {
#ifdef VECTMA_USE_OPENGL
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();

    handleInputs();

    // UI Layout
    renderSettingsMenu();
    Toolbar::render(m_stage);
    renderViewport();
    Inspector::render(m_stage);

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
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
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

} // namespace vectma
