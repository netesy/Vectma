#pragma once

#include <variant>
#include <string>
#include "core/Geometry.hpp"
#include <unordered_map>
#include "core/GColor.hpp"
#include "sync/CRDTTypes.hpp"

namespace vectma {

/**
 * @brief Type-safe variant for component property overrides.
 */
using PropertyVariant = std::variant<
    bool,
    int,
    float,
    double,
    std::string,
    GColor,
    BlendMode,
    FillType,
    StrokeAlignment,
    NodeId
>;

/**
 * @brief Maps relative node paths to their property overrides.
 * Key format: "NodeUUID.property_name"
 */
using OverrideMap = std::unordered_map<std::string, PropertyVariant>;

} // namespace vectma
