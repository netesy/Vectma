#pragma once

#include "core/CanvasNode.hpp"
#include "core/PathData.hpp"
#include <vector>
#include <memory>

namespace vectma {

enum class BooleanOpType {
    Union,
    Subtract,
    Intersect,
    Exclude
};

class CompoundShapeNode : public CanvasNode {
public:
    CompoundShapeNode(BooleanOpType op = BooleanOpType::Union);

    std::string getClassName() const override { return "CompoundShapeNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    std::string toSVG() const override;
    std::unique_ptr<CanvasNode> clone() const override;

    void setOpType(BooleanOpType op);
    BooleanOpType getOpType() const { return m_opType.value; }

    void markDirty();

    // Cache management for the resolved geometry
    bool isDirty() const { return m_pathDirty; }
    void setResolvedPath(std::unique_ptr<PathData> path) const { m_resolvedPath = std::move(path); m_pathDirty = false; }
    const PathData* getResolvedPath() const { return m_resolvedPath.get(); }

private:
    LWWProperty<BooleanOpType> m_opType;
    mutable bool m_pathDirty = true;
    mutable std::unique_ptr<PathData> m_resolvedPath;
};

} // namespace vectma
