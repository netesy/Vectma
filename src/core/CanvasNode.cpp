#include "core/CanvasNode.hpp"

namespace vectma {

CanvasNode::CanvasNode() : m_parent(nullptr) {}

CanvasNode::~CanvasNode() = default;

void CanvasNode::setParent(CanvasNode* parent) {
    m_parent = parent;
}

CanvasNode* CanvasNode::getParent() const {
    return m_parent;
}

void CanvasNode::addChild(std::unique_ptr<CanvasNode> child) {
    child->setParent(this);
    m_children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<CanvasNode>>& CanvasNode::getChildren() const {
    return m_children;
}

} // namespace vectma
