#include "AUI/Layout/ALinearLayout.hpp"
namespace aui {
ALinearLayout::ALinearLayout(Orientation orientation) : m_orientation(orientation) {}
void ALinearLayout::addView(std::shared_ptr<AView> view) { m_views.push_back(view); }
void ALinearLayout::render() {
    for (auto& view : m_views) view->render();
}
AHorizontalLayout::AHorizontalLayout() : ALinearLayout(Orientation::Horizontal) {}
AVerticalLayout::AVerticalLayout() : ALinearLayout(Orientation::Vertical) {}
}
