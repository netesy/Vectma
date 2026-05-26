#pragma once
#include "AUI/View/AView.hpp"

namespace aui {

class AModal : public AView {
public:
    virtual void open() { m_isOpen = true; }
    virtual void close() { m_isOpen = false; }
    bool isOpen() const { return m_isOpen; }

protected:
    bool m_isOpen = false;
};

} // namespace aui
