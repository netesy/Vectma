#include "ui/SplashWindow.hpp"
#include <iostream>

namespace vectma {

SplashWindow::SplashWindow() : AWindow("Splash", 1280, 720) {
    m_layout = std::make_shared<aui::AVerticalLayout>();
    m_layout->setStyle("background: #0B0B0C; align: center; justify: center;");

    auto logo = std::make_shared<aui::AButton>("VECTMA LOGO"); // Placeholder for procedural logo
    m_layout->addView(logo);

    setContentView(m_layout);
}

void SplashWindow::render() {
    if (m_contentView) m_contentView->render();
}

} // namespace vectma
