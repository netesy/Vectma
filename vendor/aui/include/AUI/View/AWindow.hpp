#pragma once
#include "AUI/View/AView.hpp"
#include <iostream>

namespace aui {

class AWindow : public AView {
public:
    AWindow(const std::string& title, int width, int height)
        : m_title(title), m_width(width), m_height(height) {}

    virtual void show() { m_isVisible = true; std::cout << "Showing window: " << m_title << std::endl; }
    virtual void hide() { m_isVisible = false; }
    bool isVisible() const { return m_isVisible; }

protected:
    std::string m_title;
    int m_width, m_height;
    bool m_isVisible = false;
    std::shared_ptr<AView> m_contentView;

public:
    void setContentView(std::shared_ptr<AView> view) { m_contentView = view; }
    std::shared_ptr<AView> getContentView() const { return m_contentView; }
};

} // namespace aui
