#pragma once

#include <vector>
#include <mutex>
#include "core/CanvasNode.hpp"
#include "core/GRect.hpp"

namespace vectma {

class SelectionManager {
public:
    SelectionManager() = default;

    void clear();
    void add(CanvasNode* node);
    void remove(CanvasNode* node);
    bool contains(CanvasNode* node) const;

    const std::vector<CanvasNode*>& getSelection() const { return m_selection; }
    size_t getCount() const { return m_selection.size(); }

    GRect getSelectionBounds() const;

    void bulkTranslate(double dx, double dy);
    void bulkScale(double sx, double sy, const GPoint& origin);

    void distributeHorizontally();
    void distributeVertically();

private:
    mutable std::mutex m_mutex;
    std::vector<CanvasNode*> m_selection;
};

} // namespace vectma
