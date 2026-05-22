#include "core/CanvasNode.hpp"
#include <algorithm>

namespace vectma {

CanvasNode::CanvasNode() : m_id({0,0,0}, {0,0,0}), m_visible(true, {0,0,0}), m_locked(false, {0,0,0}),
                           m_opacity(1.0f, {0,0,0}), m_blendMode(BlendMode::Normal, {0,0,0}),
                           m_fillType(FillType::Solid, {0,0,0}), m_fillColor(GColor::White(), {0,0,0}),
                           m_strokeAlignment(StrokeAlignment::Center, {0,0,0}), m_strokeWidth(1.0, {0,0,0}) {}

CanvasNode::~CanvasNode() = default;

void CanvasNode::setParent(CanvasNode* parent) { m_parent = parent; }
CanvasNode* CanvasNode::getParent() const { return m_parent; }

void CanvasNode::addChild(std::unique_ptr<CanvasNode> child) {
    child->setParent(this);
    m_children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<CanvasNode>>& CanvasNode::getChildren() const { return m_children; }
std::vector<std::unique_ptr<CanvasNode>>& CanvasNode::getChildrenMutable() { return m_children; }

void CanvasNode::bringToFront() {
    if (m_parent) {
        auto& sibs = m_parent->getChildrenMutable();
        auto it = std::find_if(sibs.begin(), sibs.end(), [this](const auto& p) { return p.get() == this; });
        if (it != sibs.end() && std::next(it) != sibs.end()) {
            auto node = std::move(*it);
            sibs.erase(it);
            sibs.push_back(std::move(node));
        }
    }
}

void CanvasNode::sendToBack() {
    if (m_parent) {
        auto& sibs = m_parent->getChildrenMutable();
        auto it = std::find_if(sibs.begin(), sibs.end(), [this](const auto& p) { return p.get() == this; });
        if (it != sibs.end() && it != sibs.begin()) {
            auto node = std::move(*it);
            sibs.erase(it);
            sibs.insert(sibs.begin(), std::move(node));
        }
    }
}

void CanvasNode::raiseNode() {
    if (m_parent) {
        auto& sibs = m_parent->getChildrenMutable();
        auto it = std::find_if(sibs.begin(), sibs.end(), [this](const auto& p) { return p.get() == this; });
        if (it != sibs.end() && std::next(it) != sibs.end()) std::swap(*it, *std::next(it));
    }
}

void CanvasNode::lowerNode() {
    if (m_parent) {
        auto& sibs = m_parent->getChildrenMutable();
        auto it = std::find_if(sibs.begin(), sibs.end(), [this](const auto& p) { return p.get() == this; });
        if (it != sibs.end() && it != sibs.begin()) std::swap(*it, *std::prev(it));
    }
}

} // namespace vectma
