#include "style/TokenRegistry.hpp"
#include <stdexcept>
#include <iostream>

namespace vectma {

void TokenRegistry::registerColorToken(const std::string& path, GColor lightValue, std::optional<GColor> darkValue) {
    TokenEntry entry;
    entry.name = path;
    entry.themeValues[ThemeType::Light] = TokenPrimitive(lightValue);
    entry.themeValues[ThemeType::Dark] = TokenPrimitive(darkValue.value_or(lightValue));
    m_tokens[path] = entry;
    notifyChange();
}

void TokenRegistry::registerDimensionToken(const std::string& path, double lightValue, std::optional<double> darkValue) {
    TokenEntry entry;
    entry.name = path;
    entry.themeValues[ThemeType::Light] = TokenPrimitive(lightValue);
    entry.themeValues[ThemeType::Dark] = TokenPrimitive(darkValue.value_or(lightValue));
    m_tokens[path] = entry;
    notifyChange();
}

void TokenRegistry::registerRadiusToken(const std::string& path, double lightValue, std::optional<double> darkValue) {
    registerDimensionToken(path, lightValue, darkValue);
}

void TokenRegistry::aliasToken(const std::string& path, const std::string& targetPath) {
    aliasToken(path, ThemeType::Light, targetPath);
    aliasToken(path, ThemeType::Dark, targetPath);
}

void TokenRegistry::aliasToken(const std::string& path, ThemeType theme, const std::string& targetPath) {
    if (m_tokens.find(path) == m_tokens.end()) {
        TokenEntry entry;
        entry.name = path;
        m_tokens[path] = entry;
    }
    m_tokens[path].themeValues[theme] = targetPath;
    notifyChange();
}

TokenPrimitive TokenRegistry::resolve(const std::string& path) const {
    std::set<std::string> visited;
    return resolveRecursive(path, visited, m_activeTheme);
}

TokenPrimitive TokenRegistry::resolveRecursive(const std::string& path, std::set<std::string>& visited, ThemeType theme) const {
    if (path.empty()) throw std::runtime_error("Empty token path");

    if (visited.count(path)) {
        throw std::runtime_error("Reference cycle detected in design tokens: " + path);
    }
    visited.insert(path);

    auto it = m_tokens.find(path);
    if (it == m_tokens.end()) {
        throw std::runtime_error("Token not found: " + path);
    }

    const auto& themeVals = it->second.themeValues;
    auto themeIt = themeVals.find(theme);
    if (themeIt == themeVals.end()) {
        throw std::runtime_error("Theme value not found for token: " + path);
    }

    if (std::holds_alternative<TokenPrimitive>(themeIt->second)) {
        return std::get<TokenPrimitive>(themeIt->second);
    } else {
        return resolveRecursive(std::get<std::string>(themeIt->second), visited, theme);
    }
}

void TokenRegistry::switchTheme(ThemeType newTheme) {
    if (m_activeTheme != newTheme) {
        m_activeTheme = newTheme;
        notifyChange();
    }
}

bool TokenRegistry::detectReferenceCycles() const {
    for (const auto& [path, entry] : m_tokens) {
        std::set<std::string> visited;
        try {
            resolveRecursive(path, visited, ThemeType::Light);
        } catch (const std::runtime_error&) {
            return true;
        }
        visited.clear();
        try {
            resolveRecursive(path, visited, ThemeType::Dark);
        } catch (const std::runtime_error&) {
            return true;
        }
    }
    return false;
}

void TokenRegistry::notifyChange() {
    if (m_onChange) m_onChange();
}

void TokenRegistry::clear() {
    m_tokens.clear();
}

} // namespace vectma
