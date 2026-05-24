#include "ui/EditorUI.hpp"
#include "ui/Theme.hpp"
#include "ui/Toolbar.hpp"
#include "ui/Inspector.hpp"
#include "ui/LayerPanel.hpp"
#include "ui/SplashScreen.hpp"
#include "ui/WelcomeScreen.hpp"
#include "ui/ExportDashboard.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/PathNode.hpp"
#include "core/LocaleManager.hpp"
#include "renderer/SkiaRenderPipeline.hpp"
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

    m_stage.tick(0.016); // Using fixed 60fps delta as mock imgui.h lacks it

    if (m_showSplash) {
        SplashScreen::render("logo.png");
        m_splashTimer -= 0.016f;
        if (m_splashTimer <= 0.0f) {
            m_showSplash = false;
            m_showWelcome = true;
        }
    } else if (m_showWelcome) {
        WelcomeScreen::render(m_stage, m_showWelcome);
    } else {
        handleInputs();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        float sidebarWidth = tokens::spacing::SidebarWidth;
        float toolbarHeight = tokens::spacing::ToolbarHeight;
        float statusBarHeight = tokens::spacing::StatusBarHeight;

        m_stage.setViewportSize(viewport->Size.x - 2 * sidebarWidth, viewport->Size.y - toolbarHeight - statusBarHeight);

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + toolbarHeight));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, viewport->Size.y - toolbarHeight - statusBarHeight));
        m_layerPanel->render();

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x - sidebarWidth, viewport->Pos.y + toolbarHeight));
        ImGui::SetNextWindowSize(ImVec2(sidebarWidth, viewport->Size.y - toolbarHeight - statusBarHeight));
        Inspector::render(m_stage);

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarHeight));
        Toolbar::render(m_stage);

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - statusBarHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, statusBarHeight));
        renderStatusBar();

        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + sidebarWidth, viewport->Pos.y + toolbarHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 2 * sidebarWidth, viewport->Size.y - toolbarHeight - statusBarHeight));
        renderViewport();

        renderWaypointHUD();
        renderArtboardPresets();

        ExportDashboard::render(m_stage, m_renderer, m_showExport);
        renderSettingsMenu();
    }

    ImGui::Render();
}

void EditorUI::handleInputs() {
    ImGuiIO& io = ImGui::GetIO();

    if (!io.WantCaptureMouse) {
        Point2D mousePos(io.MousePos.x, io.MousePos.y);
        bool altPressed = false;

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            m_stage.handleMouseDown(mousePos, altPressed);
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
    m_renderer.pushTransform(m_stage.getViewMatrix());
    if (m_stage.getScene()) {
        m_stage.getScene()->render(m_renderer);
    }

    if (m_stage.getTool() == ToolType::Brush && !m_stage.getActiveStroke().empty()) {
#ifdef VECTMA_USE_SKIA
        SkiaRenderPipeline* skRenderer = dynamic_cast<SkiaRenderPipeline*>(&m_renderer);
        if (skRenderer) {
            BrushSettings settings;
            settings.size = m_stage.getBrushSize();
            settings.bleeding = m_stage.getBrushBleeding();
            settings.color = tokens::colors::Primary;
            skRenderer->drawBrushStroke(m_stage.getActiveStroke(), settings);
        }
#endif
    }

    // TASK 2: Render Path Edit Handles
    if (m_stage.getEditingMode() == CanvasEditingMode::PathEdit) {
        for (auto node : m_stage.getSelection()) {
            PathNode* path = dynamic_cast<PathNode*>(node);
            if (path) {
                const auto& topology = path->getTopology();
                for (size_t i = 0; i < topology.points.size(); ++i) {
                    const auto& pt = topology.points[i];
                    m_renderer.drawAnchorOverlay(BezierAnchor(pt.position, pt.getHandleIn(), pt.getHandleOut()), (int)i == m_stage.getActiveAnchorIndex(), m_stage.getActiveHandleId());
                }
            }
        }
    }

    auto snap = m_stage.getActiveSnap();
    if (snap) {
        for (size_t i = 0; i + 1 < snap->guideLines.size(); i += 2) {
            m_renderer.drawSnappingGuide(snap->guideLines[i], snap->guideLines[i+1]);
        }
    }
    m_renderer.popTransform();
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
    if (ImGui::Button("Welcome Screen")) m_showWelcome = true;
    if (ImGui::Button("Export Hub")) m_showExport = true;
    ImGui::End();
}

void EditorUI::renderStatusBar() {
    ImGui::Begin("Status Bar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
    Point2D cursor = m_stage.getCursorCanvasPos();
    double scale = m_stage.getScale() * 100.0;
    size_t layerCount = m_stage.getSceneNodeCount();
    size_t selectionCount = m_stage.getSelection().size();
    const char* toolName = "Select";
    switch(m_stage.getTool()) {
        case ToolType::Select: toolName = "Select"; break;
        case ToolType::Marquee: toolName = "Marquee"; break;
        case ToolType::Rect: toolName = "Rect"; break;
        case ToolType::Ellipse: toolName = "Ellipse"; break;
        case ToolType::Path: toolName = "Path"; break;
        case ToolType::Text: toolName = "Text"; break;
        case ToolType::Image: toolName = "Image"; break;
        case ToolType::Brush: toolName = "Brush"; break;
        case ToolType::Pen: toolName = "Pen"; break;
        case ToolType::Artboard: toolName = "Artboard"; break;
    }
    ImGui::Text("Tool: %s | X: %.1f Y: %.1f | Zoom: %.0f%% | Layers: %zu | Selection: %zu",
                toolName, cursor.x, cursor.y, scale, layerCount, selectionCount);
    ImGui::End();
}

} // namespace vectma
