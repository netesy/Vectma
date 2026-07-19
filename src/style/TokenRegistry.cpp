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

TokenId TokenRegistry::getOrCreateTokenId(const std::string& path) {
    auto it = m_pathToId.find(path);
    if (it != m_pathToId.end()) {
        return it->second;
    }
    TokenId newId = static_cast<TokenId>(m_idToPath.size());
    m_idToPath.push_back(path);
    m_pathToId[path] = newId;
    m_resolvedCache.push_back(GColor(0, 0, 0)); // dynamic growth placeholder
    rebuildCache();
    return newId;
}

TokenId TokenRegistry::getTokenId(const std::string& path) const {
    auto it = m_pathToId.find(path);
    if (it != m_pathToId.end()) {
        return it->second;
    }
    return INVALID_TOKEN_ID;
}

std::string TokenRegistry::getTokenPath(TokenId id) const {
    if (id == INVALID_TOKEN_ID || id >= m_idToPath.size()) {
        return "";
    }
    return m_idToPath[id];
}

void TokenRegistry::rebuildCache() const {
    for (size_t id = 1; id < m_idToPath.size(); ++id) {
        try {
            std::set<std::string> visited;
            m_resolvedCache[id] = resolveRecursive(m_idToPath[id], visited, m_activeTheme);
        } catch (...) {
            // Keep safe fallback or previous value
        }
    }
}

TokenPrimitive TokenRegistry::resolve(const std::string& path) const {
    std::set<std::string> visited;
    return resolveRecursive(path, visited, m_activeTheme);
}

TokenPrimitive TokenRegistry::resolve(TokenId id) const {
    if (id == INVALID_TOKEN_ID || id >= m_resolvedCache.size()) {
        throw std::runtime_error("Invalid TokenId or cache size mismatch");
    }
    return m_resolvedCache[id];
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
    rebuildCache();
    if (m_onChange) m_onChange();
}

void TokenRegistry::clear() {
    m_tokens.clear();
    m_idToPath = {""};
    m_pathToId.clear();
    m_resolvedCache = {GColor(0, 0, 0)};
}

} // namespace vectma
