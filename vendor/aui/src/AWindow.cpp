#include "AUI/View/AWindow.hpp"
namespace aui {
AWindow::AWindow(const std::string& title, int width, int height) : m_title(title), m_width(width), m_height(height) {}
AWindow::~AWindow() = default;
void AWindow::setContentView(std::shared_ptr<AView> view) { m_contentView = view; }
void AWindow::render() { if (m_contentView) m_contentView->render(); }
}
