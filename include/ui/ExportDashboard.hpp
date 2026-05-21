#pragma once

#include "core/WorkspaceStage.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

class ExportDashboard {
public:
    static void render(WorkspaceStage& stage, RenderPipeline& renderer, bool& showExport);
};

} // namespace vectma
