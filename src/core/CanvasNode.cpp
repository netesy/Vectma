#include "core/CanvasNode.hpp"
#include <algorithm>

namespace vectma {

CanvasNode::CanvasNode() {
    LamportTimestamp ts{0, 0, 0};
    m_id.update({0, 0, 0}, ts);
    m_visible.update(true, ts);
    m_locked.update(false, ts);
    m_opacity.update(1.0f, ts);
    m_blendMode.update(BlendMode::Normal, ts);
    m_fillType.update(FillType::Solid, ts);
    m_fillColor.update(GColor::White(), ts);
    m_fillColor.tokenPath = std::nullopt;
    m_strokeAlignment.update(StrokeAlignment::Center, ts);
    m_strokeWidth.update(1.0, ts);
    m_strokeWidth.tokenPath = std::nullopt;
    m_layoutDirty = true;
}

CanvasNode::~CanvasNode() = default;

void CanvasNode::setParent(CanvasNode* parent) { m_parent = parent; }
CanvasNode* CanvasNode::getParent() const { return m_parent; }

void CanvasNode::addChild(std::unique_ptr<CanvasNode> child) {
    if (!child) return;
    child->setParent(this);
    m_children.push_back(std::move(child));
    markLayoutDirty();
}

std::unique_ptr<CanvasNode> CanvasNode::removeChild(CanvasNode* node) {
    auto it = std::find_if(m_children.begin(), m_children.end(), [node](const auto& p) { return p.get() == node; });
    if (it != m_children.end()) {
        auto removed = std::move(*it);
        m_children.erase(it);
        removed->setParent(nullptr);
        markLayoutDirty();
        return removed;
    }
    return nullptr;
}

const std::vector<std::unique_ptr<CanvasNode>>& CanvasNode::getChildren() const { return m_children; }
std::vector<std::unique_ptr<CanvasNode>>& CanvasNode::getChildrenMutable() { return m_children; }

void CanvasNode::markLayoutDirty() {
    m_layoutDirty = true;
    if (m_parent) m_parent->markLayoutDirty();
}

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

void CanvasNode::CloneBaseProperties(const CanvasNode& src, CanvasNode& dst) {
    dst.setVisibility(src.isVisible());
    dst.setLocked(src.isLocked());
    dst.setOpacity(src.getOpacity());
    dst.setBlendMode(src.getBlendMode());
    dst.setFillColor(src.getFillColor());
    dst.setStrokeWidth(src.getStrokeWidth());
    dst.setLayoutProps(src.getLayoutProps());
    dst.setHorizontalSizing(src.getHorizontalSizing());
    dst.setVerticalSizing(src.getVerticalSizing());
}

} // namespace vectma
