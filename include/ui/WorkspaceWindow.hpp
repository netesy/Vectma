#pragma once
#include "AUI/AUI.hpp"
#include "core/WorkspaceStage.hpp"
#include "renderer/RenderPipeline.hpp"

namespace vectma {

class WorkspaceWindow : public aui::AWindow {
public:
    WorkspaceWindow(WorkspaceStage& stage, RenderPipeline& renderer);

    void show() override;
    void render() override;

private:
    WorkspaceStage& m_stage;
    RenderPipeline& m_renderer;

    void setupLayout();

    std::shared_ptr<aui::AVerticalLayout> m_mainLayout;
    std::shared_ptr<aui::AHorizontalLayout> m_toolbar;
    std::shared_ptr<aui::AHorizontalLayout> m_centerLayout;
    std::shared_ptr<aui::AVerticalLayout> m_leftPanel;
    std::shared_ptr<aui::AView> m_viewport;
    std::shared_ptr<aui::AVerticalLayout> m_rightPanel;
    std::shared_ptr<aui::AHorizontalLayout> m_statusBar;
};

} // namespace vectma
