#include "ui/EditorUI.hpp"
#include "ui/Theme.hpp"
#include "ui/Toolbar.hpp"
#include "ui/Inspector.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace vectma {

EditorUI::EditorUI(WorkspaceStage& stage, RenderPipeline& renderer)
    : m_stage(stage), m_renderer(renderer) {
    Theme::applyPremiumDark();
}

void EditorUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    handleInputs();

    // UI Layout
    Toolbar::render(m_stage);

    // Viewport fills the background or a dedicated window
    renderViewport();

    Inspector::render(m_stage);

    ImGui::Render();
}

void EditorUI::handleInputs() {
    ImGuiIO& io = ImGui::GetIO();

    // Only route to stage if mouse is not captured by ImGui windows
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

    // Off-screen rendering logic would blit here
    m_renderer.beginFrame();
    if (m_stage.getScene()) {
        m_stage.getScene()->render(m_renderer);
    }
    m_renderer.endFrame();

    ImGui::End();
}

} // namespace vectma
