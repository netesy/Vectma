#pragma once
#include "AUI/AUI.hpp"
#include <functional>

namespace vectma {

class WelcomeWindow : public aui::AWindow {
public:
    WelcomeWindow();
    void render() override;

    void onNewProject(std::function<void()> cb) { m_onNewProject = cb; }
    void onOpenProject(std::function<void()> cb) { m_onOpenProject = cb; }

private:
    std::shared_ptr<aui::ALinearLayout> m_layout;
    std::function<void()> m_onNewProject;
    std::function<void()> m_onOpenProject;
};

} // namespace vectma
