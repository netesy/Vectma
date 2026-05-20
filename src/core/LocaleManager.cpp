#include "core/LocaleManager.hpp"
#include <algorithm>

namespace vectma {

const std::vector<LocaleManager::TranslationEntry> LocaleManager::s_dictionary = {
    {"toolbar.select", "Select", "Sélectionner"},
    {"toolbar.marquee", "Marquee", "Rectangle de sélection"},
    {"toolbar.rect", "Rect", "Rectangle"},
    {"toolbar.ellipse", "Ellipse", "Ellipse"},
    {"toolbar.path", "Path", "Chemin"},
    {"toolbar.text", "Text", "Texte"},
    {"toolbar.subselection", "Sub-Selection", "Sous-sélection"},
    {"inspector.type", "Type: ", "Type : "},
    {"inspector.visible", "Visible", "Visible"},
    {"inspector.stroke_alignment", "Stroke Alignment", "Alignement du contour"},
    {"inspector.typography", "Typography", "Typographie"},
    {"inspector.font_size", "Font Size", "Taille de police"},
    {"inspector.tracking", "Tracking", "Approche"},
    {"inspector.leading", "Leading", "Interlignage"},
    {"export.canvas", "Export Canvas", "Exporter le canevas"},
    {"export.format", "Format", "Format"},
    {"export.scale", "Scale", "Échelle"},
    {"export.now", "Export Now", "Exporter maintenant"}
};

LocaleManager& LocaleManager::getInstance() {
    static LocaleManager instance;
    return instance;
}

LocaleManager::LocaleManager() = default;

void LocaleManager::setLocale(Locale locale) {
    m_currentLocale = locale;
}

std::string_view LocaleManager::translate(std::string_view key) const {
    auto it = std::find_if(s_dictionary.begin(), s_dictionary.end(),
        [key](const TranslationEntry& entry) { return entry.key == key; });

    if (it != s_dictionary.end()) {
        return (m_currentLocale == Locale::FR_FR) ? it->fr : it->en;
    }
    return key;
}

} // namespace vectma
