#pragma once
#include "AUI/View/AView.hpp"
#include <functional>

namespace aui {

class AButton : public AView {
public:
    AButton(const std::string& text) : m_text(text) {}

    void onClick(std::function<void()> callback) { m_onClick = callback; }

    void render() override {}

protected:
    std::string m_text;
    std::function<void()> m_onClick;
};

} // namespace aui
