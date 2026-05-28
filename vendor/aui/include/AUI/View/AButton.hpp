#pragma once
#include "AUI/View/AView.hpp"
#include <functional>
#include <string>

namespace aui {

class AButton : public AView {
public:
    AButton(const std::string& text);
    void onClick(std::function<void()> callback);
    void render() override;

protected:
    std::string m_text;
    std::function<void()> m_onClick;
};

} // namespace aui
