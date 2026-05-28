#include "AUI/View/ATextField.hpp"
namespace aui {
ATextField::ATextField(const std::string& text) : m_text(text) {}
void ATextField::setText(const std::string& text) { m_text = text; if(m_onChanged) m_onChanged(text); }
const std::string& ATextField::getText() const { return m_text; }
void ATextField::onChanged(std::function<void(const std::string&)> callback) { m_onChanged = callback; }
void ATextField::render() {}
}
