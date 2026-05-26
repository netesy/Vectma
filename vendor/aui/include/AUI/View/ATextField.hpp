#pragma once
#include "AUI/View/AView.hpp"
#include <string>
#include <functional>

namespace aui {

class ATextField : public AView {
public:
    ATextField(const std::string& text = "") : m_text(text) {}

    void setText(const std::string& text) { m_text = text; if(m_onChanged) m_onChanged(text); }
    const std::string& getText() const { return m_text; }

    void onChanged(std::function<void(const std::string&)> callback) { m_onChanged = callback; }

    void render() override {}

private:
    std::string m_text;
    std::function<void(const std::string&)> m_onChanged;
};

} // namespace aui
