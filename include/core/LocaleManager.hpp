#pragma once

#include <string_view>
#include <string>
#include <vector>

namespace vectma {

enum class Locale {
    EN_US,
    FR_FR
};

class LocaleManager {
public:
    static LocaleManager& getInstance();

    void setLocale(Locale locale);
    Locale getLocale() const { return m_currentLocale; }

    std::string_view translate(std::string_view key) const;

private:
    LocaleManager();
    Locale m_currentLocale = Locale::EN_US;

    struct TranslationEntry {
        std::string_view key;
        std::string_view en;
        std::string_view fr;
    };

    static const std::vector<TranslationEntry> s_dictionary;
};

#define V_TXT(key) vectma::LocaleManager::getInstance().translate(key).data()

} // namespace vectma
