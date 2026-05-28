#pragma once
#include "AUI/View/AView.hpp"

namespace aui {

class AModal : public AView {
public:
    virtual void open();
    virtual void close();
    bool isOpen() const;

protected:
    bool m_isOpen = false;
};

} // namespace aui
