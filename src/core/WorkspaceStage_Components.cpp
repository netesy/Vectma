#include "core/WorkspaceStage.hpp"
#include "core/MasterComponentNode.hpp"
#include "core/ComponentInstanceNode.hpp"
#include "core/SymbolRegistry.hpp"
#include "core/Commands.hpp"

namespace vectma {

void WorkspaceStage::createComponentFromSelection() {
    if (m_selection.empty()) return;

    auto master = std::make_unique<MasterComponentNode>("Component " + std::to_string(m_selection.size()));
    std::vector<CanvasNode*> nodesToMove = m_selection;

    for (auto* node : nodesToMove) {
        if (node->getParent()) {
            auto removed = node->getParent()->removeChild(node);
            if (removed) master->addChild(std::move(removed));
        }
    }

    std::string guid = "comp_" + std::to_string(LamportClock::getInstance().tick().timestamp);
    auto* masterPtr = master.get();
    SymbolRegistry::getInstance().registerSymbol(guid, std::move(master));

    // Create an instance to replace the original selection
    auto instance = std::make_unique<ComponentInstanceNode>(masterPtr);
    executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(instance)));

    clearSelection();
}

void WorkspaceStage::detachInstance(ComponentInstanceNode* instance) {
    if (!instance || !instance->getMaster()) return;

    auto* master = instance->getMaster();
    auto* parent = dynamic_cast<SceneGraph*>(instance->getParent());
    if (!parent) return;

    // Clone all children from master into parent
    for (const auto& child : master->getChildren()) {
        auto clone = child->clone();
        parent->addChild(std::move(clone));
    }

    parent->removeChild(instance);
    clearSelection();
}

void WorkspaceStage::placeInstance(const std::string& symbolId) {
    auto* master = dynamic_cast<MasterComponentNode*>(SymbolRegistry::getInstance().getSymbolRoot(symbolId));
    if (!master) return;

    auto instance = std::make_unique<ComponentInstanceNode>(master);
    executeCommand(std::make_unique<AddNodeCommand>(m_scene.get(), std::move(instance)));
}

}
