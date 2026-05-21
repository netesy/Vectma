#include "core/SymbolRegistry.hpp"

namespace vectma {

SymbolRegistry& SymbolRegistry::getInstance() {
    static SymbolRegistry instance;
    return instance;
}

void SymbolRegistry::registerSymbol(SymbolID id, std::unique_ptr<CanvasNode> root) {
    m_symbols[id] = std::move(root);
}

CanvasNode* SymbolRegistry::getSymbolRoot(const SymbolID& id) const {
    auto it = m_symbols.find(id);
    if (it != m_symbols.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<SymbolID> SymbolRegistry::getAllSymbolIDs() const {
    std::vector<SymbolID> ids;
    for (const auto& pair : m_symbols) {
        ids.push_back(pair.first);
    }
    return ids;
}

} // namespace vectma
