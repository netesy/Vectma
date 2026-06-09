#pragma once
#include <AUI/Views.h>
#include <AUI/AWindow.h>
#include <memory>

namespace vectma {

class WorkspaceStage;
class RenderPipeline;
class ExportHub;

class WorkspaceWindow : public aui::AWindow {
public:
    WorkspaceWindow(WorkspaceStage& stage, RenderPipeline& renderer);

    void show() override;
    void render() override;

private:
    WorkspaceStage& m_stage;
    RenderPipeline& m_renderer;
    std::shared_ptr<ExportHub> m_exportHub;

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
