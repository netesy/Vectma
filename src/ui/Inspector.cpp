#include "ui/Inspector.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include <imgui.h>
#include <string>

namespace vectma {

void Inspector::render(WorkspaceStage& stage) {
    ImGui::Begin("Inspector");

    const auto& selection = stage.getSelection();
    if (selection.empty()) {
        ImGui::TextDisabled("No objects selected");
        ImGui::End();
        return;
    }

    if (selection.size() > 1) {
        ImGui::Text("%d objects selected", (int)selection.size());
    } else {
        CanvasNode* node = selection[0];
        ImGui::Text("Type: %s", node->getClassName().c_str());
        ImGui::Separator();

        bool visible = node->isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            node->setVisibility(visible);
        }

        if (dynamic_cast<RectNode*>(node)) {
            ImGui::Text("Rectangle Properties");
            // Property editing logic would go here
        } else if (dynamic_cast<EllipseNode*>(node)) {
            ImGui::Text("Ellipse Properties");
            // Property editing logic would go here
        }
    }

    ImGui::End();
}

} // namespace vectma
