#pragma once

#include <string>

namespace vectma {

class CanvasNode;
class RectNode;
class EllipseNode;
class PathNode;

/**
 * @brief Graphics hardware abstraction interface.
 * Replaces the legacy GPaintCanvas.
 */
class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;

    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;

    // Double-dispatch primitive hooks
    virtual void drawRect(const RectNode& node) = 0;
    virtual void drawEllipse(const EllipseNode& node) = 0;
    virtual void drawPath(const PathNode& node) = 0;

    // Legacy support (to be phased out)
    virtual void renderNode(const CanvasNode& node) = 0;
};

/**
 * @brief Software/Null implementation of RenderPipeline for baseline.
 */
class BaselineRenderer : public RenderPipeline {
public:
    void beginFrame() override;
    void endFrame() override;

    void drawRect(const RectNode& node) override;
    void drawEllipse(const EllipseNode& node) override;
    void drawPath(const PathNode& node) override;

    void renderNode(const CanvasNode& node) override;
};

} // namespace vectma
