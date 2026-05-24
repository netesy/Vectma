#include "core/WorkspaceStage.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/PathNode.hpp"
#include "core/Commands.hpp"

namespace vectma {

void WorkspaceStage::flattenCompoundShape(CompoundShapeNode* compound) {
    if (!compound) return;

    std::vector<BezierAnchor> allAnchors;
    for (const auto& child : compound->getChildren()) {
        if (auto* path = dynamic_cast<const PathNode*>(child.get())) {
            const auto& anchors = path->getAnchors();
            allAnchors.insert(allAnchors.end(), anchors.begin(), anchors.end());
        }
    }

    auto flatPath = std::make_unique<PathNode>(allAnchors);
    flatPath->setFillColor(compound->getFillColor());

    if (compound->getParent()) {
        auto* parent = dynamic_cast<SceneGraph*>(compound->getParent());
        if (parent) {
            parent->removeChild(compound);
            executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(flatPath)));
        }
    }
    clearSelection();
}

}
