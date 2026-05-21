#include "core/FileEngine.hpp"

#ifdef VECTMA_USE_JSON
#include <vendor/json/json.hpp>
using json = nlohmann::json;
#endif

#ifdef VECTMA_USE_MINIZIP
#include <vendor/minizip/zip.h>
#include <vendor/minizip/unzip.h>
#endif

#include <fstream>
#include <sstream>
#include <iostream>

namespace vectma {

std::future<bool> FileEngine::ExportToVectma(const std::string& filePath, const ProjectData& data) {
    return std::async(std::launch::async, [filePath, data]() {
#ifdef VECTMA_USE_MINIZIP
        zipFile zf = zipOpen(filePath.c_str(), APPEND_STATUS_CREATE);
        if (!zf) return false;

        auto writeToZip = [&](const std::string& fileName, const std::string& content) {
            zip_fileinfo zi = {0};
            if (zipOpenNewFileInZip(zf, fileName.c_str(), &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK) return false;
            if (zipWriteInFileInZip(zf, content.data(), content.size()) != ZIP_OK) return false;
            zipCloseFileInZip(zf);
            return true;
        };

        auto writeBinaryToZip = [&](const std::string& fileName, const std::vector<uint8_t>& content) {
            zip_fileinfo zi = {0};
            if (zipOpenNewFileInZip(zf, fileName.c_str(), &zi, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION) != ZIP_OK) return false;
            if (zipWriteInFileInZip(zf, content.data(), content.size()) != ZIP_OK) return false;
            zipCloseFileInZip(zf);
            return true;
        };

        if (!writeToZip("meta.json", data.metaJson)) { zipClose(zf, NULL); return false; }
        if (!writeToZip("document.json", data.documentJson)) { zipClose(zf, NULL); return false; }

        for (const auto& page : data.pagesJson) {
            if (!writeToZip("pages/" + page.first + ".json", page.second)) { zipClose(zf, NULL); return false; }
        }

        for (const auto& img : data.images) {
            if (!writeBinaryToZip("images/" + img.first, img.second)) { zipClose(zf, NULL); return false; }
        }

        zipClose(zf, NULL);
        return true;
#else
        (void)filePath; (void)data;
        return false;
#endif
    });
}

std::future<std::optional<FileEngine::ProjectData>> FileEngine::ImportFromVectma(const std::string& filePath) {
    return std::async(std::launch::async, [filePath]() -> std::optional<ProjectData> {
#ifdef VECTMA_USE_MINIZIP
        unzFile uf = unzOpen(filePath.c_str());
        if (!uf) return std::nullopt;

        ProjectData data;
        unz_global_info global_info;
        if (unzGetGlobalInfo(uf, &global_info) != UNZ_OK) { unzClose(uf); return std::nullopt; }

        auto readFile = [&](std::vector<uint8_t>& buffer) {
            unz_file_info file_info;
            char filename[256];
            if (unzGetCurrentFileInfo(uf, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK) return false;
            if (unzOpenCurrentFile(uf) != UNZ_OK) return false;
            buffer.resize(file_info.uncompressed_size);
            unzReadCurrentFile(uf, buffer.data(), buffer.size());
            unzCloseCurrentFile(uf);
            return true;
        };

        if (unzGoToFirstFile(uf) != UNZ_OK) { unzClose(uf); return std::nullopt; }

        do {
            char filename[256];
            unzGetCurrentFileInfo(uf, NULL, filename, sizeof(filename), NULL, 0, NULL, 0);
            std::string fname(filename);
            std::vector<uint8_t> buffer;
            if (!readFile(buffer)) continue;

            if (fname == "meta.json") {
                data.metaJson.assign(buffer.begin(), buffer.end());
            } else if (fname == "document.json") {
                data.documentJson.assign(buffer.begin(), buffer.end());
            } else if (fname.find("pages/") == 0 && fname.find(".json") != std::string::npos) {
                std::string pageId = fname.substr(6, fname.find(".json") - 6);
                data.pagesJson.emplace_back(pageId, std::string(buffer.begin(), buffer.end()));
            } else if (fname.find("images/") == 0) {
                std::string imgPath = fname.substr(7);
                data.images.emplace_back(imgPath, buffer);
            }
        } while (unzGoToNextFile(uf) == UNZ_OK);

        unzClose(uf);
        return data;
#else
        (void)filePath;
        return std::nullopt;
#endif
    });
}

} // namespace vectma
