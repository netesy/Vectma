#pragma once

#include "core/CanvasNode.hpp"
#include "core/GPoint.hpp"
#include "core/PathData.hpp"
#include "core/modifiers/Modifier.hpp"
#include "geometry/BezierMath.hpp"
#include <vector>
#include <string>
#include <memory>

namespace vectma {

class PathNode : public CanvasNode {
public:
    PathNode();
    PathNode(const std::vector<BezierAnchor>& anchors);
    PathNode(const geometry::PathTopology& topology);

    std::string getClassName() const override { return "PathNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    const geometry::PathTopology& getTopology() const { return m_topology; }
    void setTopology(const geometry::PathTopology& topology);

    void addAnchor(const geometry::AnchorPoint& point);
    void updateAnchor(size_t index, const geometry::AnchorPoint& point);

    // CRDT / Backend support
    void setBaseContours(const std::vector<Contour>& contours);

    bool isClosed() const { return m_topology.isClosed; }
    void setClosed(bool closed);

    // Phase 17: Modifier Pipeline
    void addModifier(std::unique_ptr<Modifier> modifier);
    const std::vector<std::unique_ptr<Modifier>>& getModifierStack() const { return m_modifier_stack; }

    void updatePipelineCache() const;
    const PathData& getCompiledPath() const;

    // Legacy support
    const std::vector<BezierAnchor>& getAnchors() const;
    void setAnchors(const std::vector<BezierAnchor>& anchors);

private:
    geometry::PathTopology m_topology;

    std::vector<std::unique_ptr<Modifier>> m_modifier_stack;

    mutable std::unique_ptr<PathData> m_cached_compiled_path;
    mutable bool m_geometry_dirty = true;
    mutable std::vector<BezierAnchor> m_legacy_anchors_cache;
};

} // namespace vectma
