#pragma once

#include "core/CanvasNode.hpp"
#include "core/GPoint.hpp"
#include "core/PathData.hpp"
#include "core/modifiers/Modifier.hpp"
#include <vector>
#include <string>
#include <memory>

namespace vectma {

class PathNode : public CanvasNode {
public:
    PathNode();
    PathNode(const std::vector<BezierAnchor>& anchors);
    PathNode(const std::vector<Contour>& contours);

    std::string getClassName() const override { return "PathNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;

    const std::vector<BezierAnchor>& getAnchors() const { return m_anchors; }
    void setAnchors(const std::vector<BezierAnchor>& anchors);

    const std::vector<Contour>& getBaseContours() const { return m_baseContours; }
    void setBaseContours(const std::vector<Contour>& contours);

    bool isClosed() const { return m_isClosed; }
    void setClosed(bool closed);

    void addAnchor(const BezierAnchor& anchor);

    // Hit testing for sub-selection
    int hitTestAnchors(const Point2D& canvasPos, float toleranceRadius) const;

    // Phase 17: Modifier Pipeline
    void addModifier(std::unique_ptr<Modifier> modifier);
    const std::vector<std::unique_ptr<Modifier>>& getModifierStack() const { return m_modifier_stack; }

    /**
     * @brief Evaluates the modifier stack and updates the cache if necessary.
     */
    void updatePipelineCache() const;

    /**
     * @brief Gets the current evaluated path data.
     */
    const PathData& getCompiledPath() const;

private:
    std::vector<BezierAnchor> m_anchors; // Legacy support
    std::vector<Contour> m_baseContours;
    bool m_isClosed = false;

    // Modifier Stack
    std::vector<std::unique_ptr<Modifier>> m_modifier_stack;

    // Cache
    mutable std::unique_ptr<PathData> m_cached_compiled_path;
    mutable bool m_geometry_dirty = true;
};

} // namespace vectma
