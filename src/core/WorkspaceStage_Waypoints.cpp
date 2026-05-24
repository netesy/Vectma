#include "core/WorkspaceStage.hpp"
#include "layout/LayoutSolver.hpp"
#include <cmath>

namespace vectma {

void WorkspaceStage::jumpToWaypoint(int slot) {
    auto wp = m_waypointManager.getWaypoint(slot);
    if (!wp) return;

    Point2D center = getViewportCenterInCanvas();
    m_startCenterX = center.x;
    m_startCenterY = center.y;
    m_startZoom = getScale();

    m_targetCenterX = wp->centerX;
    m_targetCenterY = wp->centerY;
    m_targetZoom = wp->zoomFactor;

    m_isAnimating = true;
    m_animationTime = 0.0;
}

void WorkspaceStage::tick(double dt) {
    // Phase 26: Reactive Layout Invalidation
    if (m_scene && m_scene->isLayoutDirty()) {
        LayoutSolver::ResolveConstraints(m_scene.get());
        m_scene->clearLayoutDirty();
    }

    if (!m_isAnimating) return;

    m_animationTime += dt;
    double t = std::min(1.0, m_animationTime / ANIMATION_DURATION);

    // Cubic Bezier Easing: f(t) = 3t^2 - 2t^3
    double easedT = 3 * t * t - 2 * t * t * t;

    double cx = m_startCenterX + (m_targetCenterX - m_startCenterX) * easedT;
    double cy = m_startCenterY + (m_targetCenterY - m_startCenterY) * easedT;
    double zoom = m_startZoom + (m_targetZoom - m_startZoom) * easedT;

    updateViewMatrixFromCenter(cx, cy, zoom);

    if (t >= 1.0) {
        m_isAnimating = false;
    }
}

void WorkspaceStage::updateViewMatrixFromCenter(double cx, double cy, double zoom) {
    GTransform m = GTransform::Translation(m_viewportWidth / 2.0, m_viewportHeight / 2.0)
                 .multiply(GTransform(zoom, 0, 0, zoom, 0, 0))
                 .multiply(GTransform::Translation(-cx, -cy));
    setViewMatrix(m);
}

Point2D WorkspaceStage::getViewportCenterInCanvas() const {
    return screenToCanvas(Point2D(m_viewportWidth / 2.0, m_viewportHeight / 2.0));
}

} // namespace vectma
