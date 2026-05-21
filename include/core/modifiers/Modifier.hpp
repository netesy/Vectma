#pragma once

#include "core/PathData.hpp"
#include <memory>
#include <string>

namespace vectma {

/**
 * @brief Base class for all non-destructive path modifiers.
 */
class Modifier {
public:
    virtual ~Modifier() = default;

    virtual std::string getModifierName() const = 0;

    /**
     * @brief Apply the modifier to the input path data.
     * @param input The path data to modify.
     * @return A new unique_ptr to the modified PathData.
     */
    virtual std::unique_ptr<PathData> apply(const PathData& input) const = 0;

    bool isDirty() const { return m_dirty; }
    void markDirty() { m_dirty = true; }
    void clearDirty() const { m_dirty = false; }

protected:
    mutable bool m_dirty = true;
};

} // namespace vectma
