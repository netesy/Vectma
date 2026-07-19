#pragma once

#include <string>
#include <variant>
#include <map>
#include <vector>
#include <optional>
#include <set>
#include <functional>
#include "core/GColor.hpp"

namespace vectma {

enum class ThemeType { Light, Dark };

using TokenPrimitive = std::variant<GColor, double>;

using TokenId = uint32_t;
static constexpr TokenId INVALID_TOKEN_ID = 0;

struct TokenEntry {
    std::string name;
    std::map<ThemeType, std::variant<TokenPrimitive, std::string>> themeValues;
};

class TokenRegistry {
public:
    static TokenRegistry& getInstance() {
        static TokenRegistry instance;
        return instance;
    }

    void registerColorToken(const std::string& path, GColor lightValue, std::optional<GColor> darkValue = std::nullopt);
    void registerDimensionToken(const std::string& path, double lightValue, std::optional<double> darkValue = std::nullopt);
    void registerRadiusToken(const std::string& path, double lightValue, std::optional<double> darkValue = std::nullopt);

    void aliasToken(const std::string& path, const std::string& targetPath);
    void aliasToken(const std::string& path, ThemeType theme, const std::string& targetPath);

    TokenPrimitive resolve(const std::string& path) const;
    TokenPrimitive resolve(TokenId id) const;

    template<typename T>
    T resolveAs(const std::string& path) const {
        auto val = resolve(path);
        return std::get<T>(val);
    }

    template<typename T>
    T resolveAs(TokenId id) const {
        auto val = resolve(id);
        return std::get<T>(val);
    }

    TokenId getOrCreateTokenId(const std::string& path);
    TokenId getTokenId(const std::string& path) const;
    std::string getTokenPath(TokenId id) const;

    void rebuildCache() const;

    void switchTheme(ThemeType newTheme);
    ThemeType getActiveTheme() const { return m_activeTheme; }

    bool detectReferenceCycles() const;

    void clear();

    using ChangeCallback = std::function<void()>;
    void setOnChangeCallback(ChangeCallback cb) { m_onChange = cb; }

private:
    TokenRegistry() : m_activeTheme(ThemeType::Dark) {}

    ThemeType m_activeTheme;
    std::map<std::string, TokenEntry> m_tokens;
    ChangeCallback m_onChange;

    // DOD high-performance caching vectors
    mutable std::vector<std::string> m_idToPath = {""}; // 0 is invalid
    mutable std::map<std::string, TokenId> m_pathToId;
    mutable std::vector<TokenPrimitive> m_resolvedCache = {GColor(0, 0, 0)}; // contiguous memory

    TokenPrimitive resolveRecursive(const std::string& path, std::set<std::string>& visited, ThemeType theme) const;
    void notifyChange();
};

} // namespace vectma
