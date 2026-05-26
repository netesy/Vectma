#include "prototype/PrototypePreviewStage.hpp"
#include "renderer/RenderPipeline.hpp"
#include "core/ArtboardNode.hpp"
#include "geometry/BezierMath.hpp"
#include <iostream>

namespace vectma {

PrototypePreviewStage::PrototypePreviewStage(std::shared_ptr<SceneGraph> scene)
    : m_scene(scene) {
    if (m_scene) {
        for (const auto& child : m_scene->getChildren()) {
            if (dynamic_cast<ArtboardNode*>(child.get())) {
                m_activeArtboardId = child->getId();
                break;
            }
        }
    }
}

PrototypePreviewStage::~PrototypePreviewStage() = default;

void PrototypePreviewStage::tick(double dt) {
    if (m_isTransitioning) {
        m_transitionTime += dt * 1000.0;
        if (m_transitionTime >= m_activeLink.durationMs) {
            m_isTransitioning = false;
            m_activeArtboardId = m_incomingArtboardId;
            m_incomingArtboardId = LamportTimestamp{0, 0, 0};
        }
    }
}

void PrototypePreviewStage::render(RenderPipeline& renderer) {
    if (!m_scene) return;

    ArtboardNode* active = findArtboard(m_activeArtboardId);
    if (!active) return;

    if (m_isTransitioning) {
        ArtboardNode* incoming = findArtboard(m_incomingArtboardId);
        if (incoming) {
            float t = (float)(m_transitionTime / m_activeLink.durationMs);
            float easedT = (float)geometry::BezierEvaluator::evaluateEasing(m_activeLink.easingCoefficients, t);
            renderer.drawTransition(*active, *incoming, m_activeLink.transition, easedT, m_viewportWidth, m_viewportHeight);
        } else {
            active->render(renderer);
        }
    } else {
        active->render(renderer);
    }
}

void PrototypePreviewStage::handleMouseDown(const Point2D& screenPos) {
    if (m_isTransitioning) return;

    ArtboardNode* active = findArtboard(m_activeArtboardId);
    if (!active) return;

    Point2D canvasPos = screenPos;

    auto search = [&](auto&& self, CanvasNode* node) -> CanvasNode* {
        for (auto it = node->getChildren().rbegin(); it != node->getChildren().rend(); ++it) {
            CanvasNode* found = self(self, it->get());
            if (found) return found;
        }
        if (node->containsPoint(canvasPos)) return node;
        return nullptr;
    };

    CanvasNode* hit = search(search, active);

    if (hit) {
        const PrototypeLink* link = InteractionGraph::getInstance().findLink(hit->getId(), TriggerType::MouseClick);
        if (link) {
            triggerLink(*link);
        }
    }
}

void PrototypePreviewStage::handleKeyPress(int keyCode) {
    if (m_isTransitioning) return;

    for (const auto& link : InteractionGraph::getInstance().getAllLinks()) {
        if (link.trigger == TriggerType::KeyPress && link.keyCode == keyCode) {
            triggerLink(link);
            break;
        }
    }
}

void PrototypePreviewStage::triggerLink(const PrototypeLink& link) {
    if (link.transition == TransitionType::Instant) {
        m_activeArtboardId = link.targetArtboardId;
    } else {
        m_isTransitioning = true;
        m_transitionTime = 0.0;
        m_activeLink = link;
        m_incomingArtboardId = link.targetArtboardId;
    }
}

ArtboardNode* PrototypePreviewStage::findArtboard(NodeId id) {
    if (!m_scene) return nullptr;
    for (const auto& child : m_scene->getChildren()) {
        if (child->getId() == id) {
            return dynamic_cast<ArtboardNode*>(child.get());
        }
    }
    return nullptr;
}

void PrototypePreviewStage::setInitialArtboard(NodeId artboardId) {
    m_activeArtboardId = artboardId;
}

} // namespace vectma
