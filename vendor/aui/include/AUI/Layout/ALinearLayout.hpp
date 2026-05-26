#pragma once
#include "AUI/View/AView.hpp"
#include <vector>

namespace aui {

enum class Orientation { Horizontal, Vertical };

class ALinearLayout : public AView {
public:
    ALinearLayout(Orientation orientation) : m_orientation(orientation) {}

    void addView(std::shared_ptr<AView> view) {
        m_views.push_back(view);
    }

    void render() override {
        for (auto& view : m_views) {
            view->render();
        }
    }

protected:
    Orientation m_orientation;
    std::vector<std::shared_ptr<AView>> m_views;
};

class AHorizontalLayout : public ALinearLayout {
public:
    AHorizontalLayout() : ALinearLayout(Orientation::Horizontal) {}
};

class AVerticalLayout : public ALinearLayout {
public:
    AVerticalLayout() : ALinearLayout(Orientation::Vertical) {}
};

} // namespace aui
