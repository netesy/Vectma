#include "core/PathNode.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

PathNode::PathNode(const std::string& pathData)
    : m_pathData(pathData) {}

void PathNode::render(RenderPipeline& pipeline) const {
    pipeline.drawPath(*this);
}

bool PathNode::containsPoint(const GPoint& point) const {
    (void)point;
    return false;
}

GRect PathNode::computeBoundingBox() const {
    return GRect(0, 0, 0, 0);
}

} // namespace vectma
