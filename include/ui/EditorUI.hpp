#pragma once

#include "core/WorkspaceStage.hpp"
#include "renderer/RenderPipeline.hpp"
#include "ui/LayerPanel.hpp"
#include "ui/SplashScreen.hpp"
#include <memory>

namespace vectma {

class EditorUI {
public:
    EditorUI(WorkspaceStage& stage, RenderPipeline& renderer);

    void render();

private:
    WorkspaceStage& m_stage;
    RenderPipeline& m_renderer;
    std::unique_ptr<LayerPanel> m_layerPanel;
    bool m_showSplash = true;
    float m_splashTimer = 3.0f;

    void handleInputs();
    void renderViewport();
    void renderSettingsMenu();
    void renderStatusBar();
};

} // namespace vectma
