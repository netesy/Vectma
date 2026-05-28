#pragma once
#include "AUI/View/AView.hpp"
#include <string>
#include <functional>

namespace aui {

class ATextField : public AView {
public:
    ATextField(const std::string& text = "");
    void setText(const std::string& text);
    const std::string& getText() const;
    void onChanged(std::function<void(const std::string&)> callback);
    void render() override;

private:
    std::string m_text;
    std::function<void(const std::string&)> m_onChanged;
};

} // namespace aui
