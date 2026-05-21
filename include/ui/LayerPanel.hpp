#pragma once

#include <imgui.h>
#include <string>

namespace vectma {

class CanvasNode;
class WorkspaceStage;

class LayerPanel {
public:
    LayerPanel(WorkspaceStage& stage);

    void render();

private:
    void renderNodeRecursive(CanvasNode* node);

    WorkspaceStage& m_stage;
};

} // namespace vectma
