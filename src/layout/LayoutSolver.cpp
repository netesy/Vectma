#include "layout/LayoutSolver.hpp"
#include "core/CanvasNode.hpp"
#include "core/SceneGraph.hpp"
#include "style/TokenRegistry.hpp"
#include <algorithm>
#include <vector>
#include <iostream>

namespace vectma {

void LayoutSolver::ResolveConstraints(CanvasNode* root) {
    if (!root) return;
    measureNode(root);

    float startW = root->m_prefWidth;
    float startH = root->m_prefHeight;

    if (root->getHorizontalSizing() != SizingRule::HugContents) startW = (float)root->getWidth();
    if (root->getVerticalSizing() != SizingRule::HugContents) startH = (float)root->getHeight();

    layoutNode(root, startW, startH);

    // After resolving layout, sync the calculated layout attributes into the contiguous LayoutSoACache
    SceneGraph* sg = dynamic_cast<SceneGraph*>(root);
    if (sg) {
        sg->updateSoACache();
    }
}

void LayoutSolver::measureNode(CanvasNode* node) {
    if (!node) return;

    for (auto& child : node->getChildren()) {
        measureNode(child.get());
    }

    const auto& props = node->getLayoutProps();
    if (!props.enabled) {
        node->m_prefWidth = (float)node->getWidth();
        node->m_prefHeight = (float)node->getHeight();
        if (node->m_prefWidth <= 0 || node->m_prefHeight <= 0) {
            GRect bbox = node->computeBoundingBox();
            node->m_prefWidth = (float)bbox.width;
            node->m_prefHeight = (float)bbox.height;
        }
        return;
    }

    float gap = props.gap;
    if (props.gapTokenPath) {
        try {
            gap = (float)TokenRegistry::getInstance().resolveAs<double>(*props.gapTokenPath);
        } catch (const std::exception& e) {
            std::cerr << "LayoutSolver Error resolving gap token: " << e.what() << std::endl;
        }
    }

    float totalMain = 0;
    float maxCounter = 0;
    int visibleCount = 0;

    for (const auto& child : node->getChildren()) {
        if (!child->isVisible()) continue;
        visibleCount++;

        float cw = child->m_prefWidth;
        float ch = child->m_prefHeight;

        if (props.direction == LayoutDirection::Horizontal) {
            totalMain += cw;
            maxCounter = std::max(maxCounter, ch);
        } else {
            totalMain += ch;
            maxCounter = std::max(maxCounter, cw);
        }
    }

    if (visibleCount > 1) totalMain += (visibleCount - 1) * gap;

    float hPad = props.padding.left + props.padding.right;
    float vPad = props.padding.top + props.padding.bottom;

    if (node->getHorizontalSizing() == SizingRule::HugContents) {
        node->m_prefWidth = (props.direction == LayoutDirection::Horizontal) ? (totalMain + hPad) : (maxCounter + hPad);
    } else {
        node->m_prefWidth = (float)node->getWidth();
    }

    if (node->getVerticalSizing() == SizingRule::HugContents) {
        node->m_prefHeight = (props.direction == LayoutDirection::Horizontal) ? (maxCounter + vPad) : (totalMain + vPad);
    } else {
        node->m_prefHeight = (float)node->getHeight();
    }
}

void LayoutSolver::layoutNode(CanvasNode* node, float availW, float availH) {
    if (!node) return;
    const auto& props = node->getLayoutProps();
    if (!props.enabled) return;

    float gap = props.gap;
    if (props.gapTokenPath) {
        try {
            gap = (float)TokenRegistry::getInstance().resolveAs<double>(*props.gapTokenPath);
        } catch (const std::exception& e) {
            std::cerr << "LayoutSolver Error resolving gap token in layout: " << e.what() << std::endl;
        }
    }

    float innerW = availW - props.padding.left - props.padding.right;
    float innerH = availH - props.padding.top - props.padding.bottom;

    float mainSpace = (props.direction == LayoutDirection::Horizontal) ? innerW : innerH;
    float fixedTotal = 0;
    int flexCount = 0;
    int visibleCount = 0;

    for (const auto& child : node->getChildren()) {
        if (!child->isVisible()) continue;
        visibleCount++;
        SizingRule mainSizing = (props.direction == LayoutDirection::Horizontal) ? child->getHorizontalSizing() : child->getVerticalSizing();
        if (mainSizing == SizingRule::FillContainer) {
            flexCount++;
        } else {
            fixedTotal += (props.direction == LayoutDirection::Horizontal) ? child->m_prefWidth : child->m_prefHeight;
        }
    }

    if (visibleCount > 1) fixedTotal += (visibleCount - 1) * gap;
    float remaining = std::max(0.0f, mainSpace - fixedTotal);
    float flexSize = (flexCount > 0) ? (remaining / flexCount) : 0;

    float currentX = props.padding.left;
    float currentY = props.padding.top;
    float parentX = (float)node->getX();
    float parentY = (float)node->getY();

    LamportTimestamp ts = LamportClock::getInstance().tick();

    for (auto& child : node->getChildren()) {
        if (!child->isVisible()) continue;

        float targetW = child->m_prefWidth;
        float targetH = child->m_prefHeight;

        if (props.direction == LayoutDirection::Horizontal) {
            if (child->getHorizontalSizing() == SizingRule::FillContainer) targetW = flexSize;
            if (child->getVerticalSizing() == SizingRule::FillContainer) targetH = innerH;
        } else {
            if (child->getVerticalSizing() == SizingRule::FillContainer) targetH = flexSize;
            if (child->getHorizontalSizing() == SizingRule::FillContainer) targetW = innerW;
        }

        float posX = parentX + currentX;
        float posY = parentY + currentY;

        child->setPositionRemote(posX, posY, ts);
        child->setSizeRemote(targetW, targetH, ts);

        if (props.direction == LayoutDirection::Horizontal) currentX += targetW + gap;
        else currentY += targetH + gap;

        layoutNode(child.get(), targetW, targetH);
    }
}

} // namespace vectma
