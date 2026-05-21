#include "ui/LayerPanel.hpp"
#include "core/CanvasNode.hpp"
#include "core/WorkspaceStage.hpp"
#include "core/SceneGraph.hpp"
#include <imgui.h>

namespace vectma {

LayerPanel::LayerPanel(WorkspaceStage& stage) : m_stage(stage) {}

void LayerPanel::render() {
    ImGui::Begin("Layers");

    auto scene = m_stage.getScene();
    if (scene) {
        for (const auto& child : scene->getChildren()) {
            renderNodeRecursive(child.get());
        }
    }

    ImGui::End();
}

void LayerPanel::renderNodeRecursive(CanvasNode* node) {
    if (ImGui::CollapsingHeader(node->getClassName().c_str())) {
        ImGui::SameLine();

        bool visible = node->isVisible();
        if (ImGui::Checkbox(("V##" + std::to_string((uintptr_t)node)).c_str(), &visible)) {
            node->setVisibility(visible);
        }

        ImGui::SameLine();
        bool locked = node->isLocked();
        if (ImGui::Checkbox(("L##" + std::to_string((uintptr_t)node)).c_str(), &locked)) {
            node->setLocked(locked);
        }

        float opacity = node->getOpacity();
        if (ImGui::SliderFloat(("Opacity##" + std::to_string((uintptr_t)node)).c_str(), &opacity, 0.0f, 1.0f)) {
            node->setOpacity(opacity);
        }

        for (const auto& child : node->getChildren()) {
            ImGui::PushID((int)(uintptr_t)child.get());
            renderNodeRecursive(child.get());
            ImGui::PopID();
        }
    }
}

} // namespace vectma
