#pragma once

#include <string>
#include <vector>
#include <memory>
#include <future>
#include <optional>
#include "core/SceneGraph.hpp"

namespace vectma {

/**
 * @brief Thread-safe, asynchronous file pipeline for *.vectma project bundles.
 */
class FileEngine {
public:
    struct ProjectData {
        std::string metaJson;
        std::string documentJson;
        std::vector<std::pair<std::string, std::string>> pagesJson; // id, content
        std::vector<std::pair<std::string, std::vector<uint8_t>>> images; // path, data
    };

    /**
     * @brief Exports the current scene graph and project state to a .vectma archive.
     */
    static std::future<bool> ExportToVectma(const std::string& filePath, const ProjectData& data);

    /**
     * @brief Imports a project from a .vectma archive.
     */
    static std::future<std::optional<ProjectData>> ImportFromVectma(const std::string& filePath);
};

} // namespace vectma
