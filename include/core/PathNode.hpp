#pragma once

#include "core/CanvasNode.hpp"
#include <string>

namespace vectma {

class PathNode : public CanvasNode {
public:
    PathNode(const std::string& pathData);

    std::string getClassName() const override { return "PathNode"; }

    void render(RenderPipeline& pipeline) const override;
    bool containsPoint(const GPoint& point) const override;
    GRect computeBoundingBox() const override;

    const std::string& getPathData() const { return m_pathData; }

private:
    std::string m_pathData;
};

} // namespace vectma
