#pragma once

#include "core/SceneGraph.hpp"
#include "core/GTransform.hpp"
#include "prototype/InteractionGraph.hpp"
#include <memory>
#include <vector>

namespace vectma {

class ArtboardNode;

class PrototypePreviewStage {
public:
    PrototypePreviewStage(std::shared_ptr<SceneGraph> scene);
    ~PrototypePreviewStage();

    void tick(double dt);
    void render(class RenderPipeline& renderer);

    void handleMouseDown(const Point2D& screenPos);
    void handleKeyPress(int keyCode);

    void setViewportSize(double w, double h) { m_viewportWidth = w; m_viewportHeight = h; }
    void setInitialArtboard(NodeId artboardId);

private:
    std::shared_ptr<SceneGraph> m_scene;
    NodeId m_activeArtboardId;
    NodeId m_incomingArtboardId;

    double m_viewportWidth = 1280;
    double m_viewportHeight = 720;

    // Transition state
    bool m_isTransitioning = false;
    double m_transitionTime = 0.0;
    PrototypeLink m_activeLink;

    void triggerLink(const PrototypeLink& link);
    ArtboardNode* findArtboard(NodeId id);
};

} // namespace vectma
