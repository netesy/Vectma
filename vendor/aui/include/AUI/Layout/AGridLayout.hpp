#pragma once
#include "AUI/View/AView.hpp"

namespace aui {

class AGridLayout : public AView {
public:
    AGridLayout(int columns, int rows) : m_columns(columns), m_rows(rows) {}

    void addView(std::shared_ptr<AView> view, int x, int y) {
        m_cells.push_back({view, x, y});
    }

    void render() override {
        for (auto& cell : m_cells) {
            cell.view->render();
        }
    }

private:
    struct Cell {
        std::shared_ptr<AView> view;
        int x, y;
    };
    int m_columns, m_rows;
    std::vector<Cell> m_cells;
};

} // namespace aui
