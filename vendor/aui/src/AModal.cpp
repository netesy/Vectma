#include "AUI/View/AModal.hpp"
namespace aui {
void AModal::open() { m_isOpen = true; }
void AModal::close() { m_isOpen = false; }
bool AModal::isOpen() const { return m_isOpen; }
}
