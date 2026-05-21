#pragma once

#include "core/CanvasNode.hpp"
#include <string>
#include <map>
#include <memory>

namespace vectma {

using SymbolID = std::string;

struct SymbolDefinition {
    SymbolID id;
    std::unique_ptr<CanvasNode> root;
};

class SymbolRegistry {
public:
    static SymbolRegistry& getInstance();

    void registerSymbol(SymbolID id, std::unique_ptr<CanvasNode> root);
    CanvasNode* getSymbolRoot(const SymbolID& id) const;
    std::vector<SymbolID> getAllSymbolIDs() const;

private:
    SymbolRegistry() = default;
    std::map<SymbolID, std::unique_ptr<CanvasNode>> m_symbols;
};

} // namespace vectma
