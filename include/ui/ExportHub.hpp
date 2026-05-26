#pragma once
#include "AUI/AUI.hpp"
#include "core/WorkspaceStage.hpp"
#include <functional>

namespace vectma {

class ExportHub : public aui::AModal {
public:
    ExportHub(WorkspaceStage& stage);
    void render() override;

    void onExport(std::function<void(const std::string& format)> cb) { m_onExport = cb; }

private:
    WorkspaceStage& m_stage;
    std::shared_ptr<aui::AVerticalLayout> m_layout;
    std::function<void(const std::string& format)> m_onExport;
};

} // namespace vectma
