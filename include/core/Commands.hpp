#pragma once

#include "core/Command.hpp"
#include "core/CanvasNode.hpp"
#include "core/SceneGraph.hpp"
#include "core/PathNode.hpp"
#include "core/modifiers/CornerRoundingModifier.hpp"
#include <memory>

namespace vectma {

class AddNodeCommand : public Command {
public:
    AddNodeCommand(SceneGraph* scene, std::unique_ptr<CanvasNode> node)
        : m_scene(scene), m_node(std::move(node)) {}

    void execute() override {
        m_node_ptr = m_node.get();
        m_scene->addChild(std::move(m_node));
    }

    void undo() override {
        m_node = m_scene->removeChild(m_node_ptr);
    }

    void redo() override { execute(); }

private:
    SceneGraph* m_scene;
    std::unique_ptr<CanvasNode> m_node;
    CanvasNode* m_node_ptr = nullptr;
};

class ModifyPathEffectsCommand : public Command {
public:
    ModifyPathEffectsCommand(PathNode* node, float oldRad, float newRad)
        : m_node(node), m_oldRad(oldRad), m_newRad(newRad) {}

    void execute() override {
        for (const auto& mod : m_node->getModifierStack()) {
            if (auto roundMod = dynamic_cast<CornerRoundingModifier*>(mod.get())) {
                roundMod->setRadius(m_newRad);
                return;
            }
        }
        auto roundMod = std::make_unique<CornerRoundingModifier>(m_newRad);
        m_node->addModifier(std::move(roundMod));
    }

    void undo() override {
        for (const auto& mod : m_node->getModifierStack()) {
            if (auto roundMod = dynamic_cast<CornerRoundingModifier*>(mod.get())) {
                roundMod->setRadius(m_oldRad);
                return;
            }
        }
    }

    void redo() override { execute(); }

private:
    PathNode* m_node;
    float m_oldRad, m_newRad;
};

} // namespace vectma
