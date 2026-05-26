#pragma once
#include "AUI/View/AView.hpp"

namespace aui {

class AWindow : public AView {
public:
    AWindow(const std::string& title, int width, int height)
        : m_title(title), m_width(width), m_height(height) {}

    virtual void show() = 0;

protected:
    std::string m_title;
    int m_width, m_height;
    std::shared_ptr<AView> m_contentView;

public:
    void setContentView(std::shared_ptr<AView> view) { m_contentView = view; }
};

} // namespace aui
