#include "AUI/View/AButton.hpp"
namespace aui {
AButton::AButton(const std::string& text) : m_text(text) {}
void AButton::onClick(std::function<void()> callback) { m_onClick = callback; }
void AButton::render() {}
}
