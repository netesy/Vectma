#pragma once
#include "AUI/AUI.hpp"
#include "core/CanvasNode.hpp"
#include "core/SceneGraph.hpp"

namespace vectma {

class LayerListModel : public aui::AAbstractListModel<CanvasNode*> {
public:
    LayerListModel(std::shared_ptr<SceneGraph> scene) : m_scene(scene) {}

    size_t rowCount() const override {
        return m_scene ? m_scene->getChildren().size() : 0;
    }

    CanvasNode* data(size_t index) const override {
        if (!m_scene || index >= m_scene->getChildren().size()) return nullptr;
        return m_scene->getChildren()[index].get();
    }

private:
    std::shared_ptr<SceneGraph> m_scene;
};

} // namespace vectma
