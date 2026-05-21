#pragma once

#include "core/WorkspaceStage.hpp"
#include <string>
#include <vector>

namespace vectma {

class WelcomeScreen {
public:
    static void render(WorkspaceStage& stage, bool& showWelcome);

private:
    static void renderRecentFiles();
    static void renderTemplates();
};

} // namespace vectma
