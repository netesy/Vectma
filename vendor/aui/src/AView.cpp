#include "AUI/View/AView.hpp"
namespace aui {
AView::~AView() = default;
void AView::render() {}
void AView::setStyle(const std::string& style) { m_style = style; }
void AView::onMouseDown(float x, float y) { (void)x; (void)y; }
void AView::onMouseMove(float x, float y) { (void)x; (void)y; }
void AView::onMouseUp() {}
}
