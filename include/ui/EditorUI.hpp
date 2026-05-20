#pragma once

#include "core/WorkspaceStage.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

class EditorUI {
public:
    EditorUI(WorkspaceStage& stage, RenderPipeline& renderer);

    void render();

private:
    WorkspaceStage& m_stage;
    RenderPipeline& m_renderer;

    void handleInputs();
    void renderViewport();
};

} // namespace vectma
