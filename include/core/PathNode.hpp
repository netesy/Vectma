#pragma once

#include "core/CanvasNode.hpp"
#include "core/GPoint.hpp"
#include <vector>
#include <string>

namespace vectma {

class PathNode : public CanvasNode {
public:
    PathNode();
    PathNode(const std::vector<BezierAnchor>& anchors);

    std::string getClassName() const override { return "PathNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;

    const std::vector<BezierAnchor>& getAnchors() const { return m_anchors; }
    void setAnchors(const std::vector<BezierAnchor>& anchors) { m_anchors = anchors; }

    void addAnchor(const BezierAnchor& anchor) { m_anchors.push_back(anchor); }

    // Hit testing for sub-selection
    int hitTestAnchors(const Point2D& canvasPos, float toleranceRadius) const;

    // Phase 12: Effect Attributes
    float cornerRadius = 0.0f;
    std::vector<float> dashPattern;
    float strokeOffset = 0.0f;

private:
    std::vector<BezierAnchor> m_anchors;
};

} // namespace vectma
