#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/GPoint.hpp"
#include "core/GRect.hpp"

namespace vectma {

class RenderPipeline;

/**
 * @brief Abstract polymorphic base class for all scene objects.
 * Replaces the legacy GObject.
 */
class CanvasNode {
public:
    CanvasNode();
    virtual ~CanvasNode();

    // Disable copying for unique ownership management
    CanvasNode(const CanvasNode&) = delete;
    CanvasNode& operator=(const CanvasNode&) = delete;

    virtual std::string getClassName() const = 0;

    void setParent(CanvasNode* parent);
    CanvasNode* getParent() const;

    void addChild(std::unique_ptr<CanvasNode> child);
    const std::vector<std::unique_ptr<CanvasNode>>& getChildren() const;

    // Visibility
    bool isVisible() const { return m_visible; }
    void setVisibility(bool visible) { m_visible = visible; }

    // Dispatch Anchor for Visitor Pattern
    virtual void render(RenderPipeline& pipeline) const = 0;

    // Geometric Interfaces
    virtual bool containsPoint(const GPoint& point) const = 0;
    virtual GRect computeBoundingBox() const = 0;

protected:
    CanvasNode* m_parent = nullptr;
    std::vector<std::unique_ptr<CanvasNode>> m_children;
    bool m_visible = true;
};

} // namespace vectma
