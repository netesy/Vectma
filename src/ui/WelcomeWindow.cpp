#include "ui/WelcomeWindow.hpp"
#include <iostream>

namespace vectma {

WelcomeWindow::WelcomeWindow() : AWindow("Welcome", 1280, 720) {
    m_layout = std::make_shared<aui::AVerticalLayout>();
    m_layout->setStyle("background: #131314; padding: 48px;");

    auto title = std::make_shared<aui::AButton>("Welcome to Vectma");
    m_layout->addView(title);

    auto newBtn = std::make_shared<aui::AButton>("Create New Project");
    newBtn->onClick([this]() { if(m_onNewProject) m_onNewProject(); });
    m_layout->addView(newBtn);

    auto openBtn = std::make_shared<aui::AButton>("Open Existing Project");
    openBtn->onClick([this]() { if(m_onOpenProject) m_onOpenProject(); });
    m_layout->addView(openBtn);

    setContentView(m_layout);
}

void WelcomeWindow::render() {
    if (m_contentView) m_contentView->render();
}

} // namespace vectma
