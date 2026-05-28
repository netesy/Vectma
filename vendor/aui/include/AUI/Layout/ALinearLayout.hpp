#pragma once
#include "AUI/View/AView.hpp"
#include <vector>
#include <memory>

namespace aui {

enum class Orientation { Horizontal, Vertical };

class ALinearLayout : public AView {
public:
    ALinearLayout(Orientation orientation);
    void addView(std::shared_ptr<AView> view);
    void render() override;

protected:
    Orientation m_orientation;
    std::vector<std::shared_ptr<AView>> m_views;
};

class AHorizontalLayout : public ALinearLayout {
public:
    AHorizontalLayout();
};

class AVerticalLayout : public ALinearLayout {
public:
    AVerticalLayout();
};

} // namespace aui
