#include "ui/SelectionManager.hpp"
#include "sync/CRDTTypes.hpp"
#include <algorithm>

namespace vectma {

void SelectionManager::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_selection.clear();
}

void SelectionManager::add(CanvasNode* node) {
    if (!node) return;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (std::find(m_selection.begin(), m_selection.end(), node) == m_selection.end()) {
        m_selection.push_back(node);
    }
}

void SelectionManager::remove(CanvasNode* node) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find(m_selection.begin(), m_selection.end(), node);
    if (it != m_selection.end()) {
        m_selection.erase(it);
    }
}

bool SelectionManager::contains(CanvasNode* node) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return std::find(m_selection.begin(), m_selection.end(), node) != m_selection.end();
}

GRect SelectionManager::getSelectionBounds() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_selection.empty()) return GRect(0, 0, 0, 0);

    GRect bounds = m_selection[0]->computeBoundingBox();
    for (size_t i = 1; i < m_selection.size(); ++i) {
        bounds = bounds.united(m_selection[i]->computeBoundingBox());
    }
    return bounds;
}

void SelectionManager::bulkTranslate(double dx, double dy) {
    std::lock_guard<std::mutex> lock(m_mutex);
    LamportTimestamp ts = LamportClock::getInstance().tick();
    for (auto node : m_selection) {
        node->setPositionRemote(node->getX() + dx, node->getY() + dy, ts);
    }
}

void SelectionManager::bulkScale(double sx, double sy, const GPoint& origin) {
    std::lock_guard<std::mutex> lock(m_mutex);
    LamportTimestamp ts = LamportClock::getInstance().tick();
    for (auto node : m_selection) {
        double newX = origin.x + (node->getX() - origin.x) * sx;
        double newY = origin.y + (node->getY() - origin.y) * sy;
        double newW = node->getWidth() * sx;
        double newH = node->getHeight() * sy;
        node->setPositionRemote(newX, newY, ts);
        node->setSizeRemote(newW, newH, ts);
    }
}

void SelectionManager::distributeHorizontally() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_selection.size() < 3) return;

    std::vector<CanvasNode*> sorted = m_selection;
    std::sort(sorted.begin(), sorted.end(), [](CanvasNode* a, CanvasNode* b) {
        return a->getX() < b->getX();
    });

    double minX = sorted.front()->getX();
    double maxX = sorted.back()->getX() + sorted.back()->getWidth();
    double totalWidths = 0;
    for (auto node : sorted) totalWidths += node->getWidth();

    double totalGap = (maxX - minX) - totalWidths;
    double gap = totalGap / (sorted.size() - 1);

    double currentX = minX;
    LamportTimestamp ts = LamportClock::getInstance().tick();
    for (size_t i = 0; i < sorted.size(); ++i) {
        sorted[i]->setPositionRemote(currentX, sorted[i]->getY(), ts);
        currentX += sorted[i]->getWidth() + gap;
    }
}

void SelectionManager::distributeVertically() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_selection.size() < 3) return;

    std::vector<CanvasNode*> sorted = m_selection;
    std::sort(sorted.begin(), sorted.end(), [](CanvasNode* a, CanvasNode* b) {
        return a->getY() < b->getY();
    });

    double minY = sorted.front()->getY();
    double maxY = sorted.back()->getY() + sorted.back()->getHeight();
    double totalHeights = 0;
    for (auto node : sorted) totalHeights += node->getHeight();

    double totalGap = (maxY - minY) - totalHeights;
    double gap = totalGap / (sorted.size() - 1);

    double currentY = minY;
    LamportTimestamp ts = LamportClock::getInstance().tick();
    for (size_t i = 0; i < sorted.size(); ++i) {
        sorted[i]->setPositionRemote(sorted[i]->getX(), currentY, ts);
        currentY += sorted[i]->getHeight() + gap;
    }
}

} // namespace vectma
