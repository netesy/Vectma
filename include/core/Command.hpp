#pragma once

namespace vectma {

/**
 * @brief Abstract base class for the Command Pattern transaction lifecycle.
 */
class Command {
public:
    virtual ~Command() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() = 0;
};

} // namespace vectma
