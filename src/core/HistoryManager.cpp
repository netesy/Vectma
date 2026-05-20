#include "core/HistoryManager.hpp"

namespace vectma {

HistoryManager::HistoryManager(size_t maxDepth)
    : m_maxHistoryDepth(maxDepth) {}

void HistoryManager::executeCommand(std::unique_ptr<Command> cmd) {
    cmd->execute();
    m_undoStack.push_back(std::move(cmd));
    m_redoStack.clear(); // Flush redo stack on new command

    if (m_undoStack.size() > m_maxHistoryDepth) {
        m_undoStack.erase(m_undoStack.begin());
    }
}

bool HistoryManager::undo() {
    if (m_undoStack.empty()) return false;

    auto cmd = std::move(m_undoStack.back());
    m_undoStack.pop_back();

    cmd->undo();
    m_redoStack.push_back(std::move(cmd));
    return true;
}

bool HistoryManager::redo() {
    if (m_redoStack.empty()) return false;

    auto cmd = std::move(m_redoStack.back());
    m_redoStack.pop_back();

    cmd->redo();
    m_undoStack.push_back(std::move(cmd));
    return true;
}

} // namespace vectma
