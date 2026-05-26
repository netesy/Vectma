#pragma once
#include "AUI/AUI.hpp"

namespace vectma {

class SplashWindow : public aui::AWindow {
public:
    SplashWindow();
    void render() override;

    bool isFinished() const { return m_timer <= 0.0f; }
    void tick(float dt) { m_timer -= dt; }

private:
    float m_timer = 3.0f;
    std::shared_ptr<aui::ALinearLayout> m_layout;
};

} // namespace vectma
