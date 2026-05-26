#include "ui/ExportHub.hpp"
#include <iostream>

namespace vectma {

ExportHub::ExportHub(WorkspaceStage& stage) : m_stage(stage) {
    m_layout = std::make_shared<aui::AVerticalLayout>();
    m_layout->setStyle("background: rgba(30, 30, 30, 0.9); padding: 20px; border-radius: 8px;");

    auto title = std::make_shared<aui::AButton>("Export & Handoff Hub");
    m_layout->addView(title);

    auto pngBtn = std::make_shared<aui::AButton>("Export PNG");
    pngBtn->onClick([this]() { if(m_onExport) m_onExport("PNG"); close(); });
    m_layout->addView(pngBtn);

    auto svgBtn = std::make_shared<aui::AButton>("Export SVG");
    svgBtn->onClick([this]() { if(m_onExport) m_onExport("SVG"); close(); });
    m_layout->addView(svgBtn);

    auto closeBtn = std::make_shared<aui::AButton>("Close");
    closeBtn->onClick([this]() { close(); });
    m_layout->addView(closeBtn);
}

void ExportHub::render() {
    if (m_isOpen && m_layout) {
        m_layout->render();
    }
}

} // namespace vectma
