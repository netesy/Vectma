#pragma once

#include "core/Command.hpp"
#include <vector>
#include <memory>

namespace vectma {

class HistoryManager {
public:
    HistoryManager(size_t maxDepth = 100);

    void executeCommand(std::unique_ptr<Command> cmd);
    bool undo();
    bool redo();

    bool canUndo() const { return !m_undoStack.empty(); }
    bool canRedo() const { return !m_redoStack.empty(); }

private:
    std::vector<std::unique_ptr<Command>> m_undoStack;
    std::vector<std::unique_ptr<Command>> m_redoStack;
    size_t m_maxHistoryDepth;
};

} // namespace vectma
