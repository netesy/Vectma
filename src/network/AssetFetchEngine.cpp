#include "network/AssetFetchEngine.hpp"
#include "vendor/json/json.hpp"
#include <cstdio>
#include <iostream>
#include <sstream>
using json = nlohmann::json;
namespace vectma { namespace network {
AssetFetchEngine::AssetFetchEngine() : m_running(true) { m_worker = std::thread(&AssetFetchEngine::workerLoop, this); }
AssetFetchEngine::~AssetFetchEngine() { stop(); }
void AssetFetchEngine::stop() { if (m_running) { m_running = false; m_cv.notify_all(); if (m_worker.joinable()) m_worker.join(); } }
void AssetFetchEngine::fetchIndex(IndexCallback cb) { std::lock_guard<std::mutex> lock(m_queueMutex); m_tasks.push({Task::FetchIndex, "", cb, nullptr}); m_cv.notify_one(); }
void AssetFetchEngine::fetchSVG(const std::string& slug, SVGCallback cb) { std::lock_guard<std::mutex> lock(m_queueMutex); m_tasks.push({Task::FetchSVG, slug, nullptr, cb}); m_cv.notify_one(); }
void AssetFetchEngine::workerLoop() {
    while (m_running) {
        Task task;
        { std::unique_lock<std::mutex> lock(m_queueMutex); m_cv.wait(lock, [this] { return !m_tasks.empty() || !m_running; }); if (!m_running && m_tasks.empty()) break; task = std::move(m_tasks.front()); m_tasks.pop(); }
        if (task.type == Task::FetchIndex) {
            std::string data = performGet("https://cdn.jsdelivr.net/gh/glincker/thesvg@main/data/icons.json");
            std::vector<AssetInfo> assets;
            try { auto j = json::parse(data); for (auto& item : j) assets.push_back({item["name"], item["slug"], item["category"]}); } catch (...) {}
            if (task.indexCb) task.indexCb(assets);
        } else if (task.type == Task::FetchSVG) {
            std::string data = performGet("https://cdn.jsdelivr.net/gh/glincker/thesvg@main/public/icons/" + task.slug + "/default.svg");
            if (task.svgCb) task.svgCb(task.slug, parseSVGPath(data));
        }
    }
}
std::string AssetFetchEngine::performGet(const std::string& url) {
    std::string cmd = "curl -s \"" + url + "\"";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    char buffer[4096]; std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) result += buffer;
    pclose(pipe); return result;
}
geometry::PathTopology AssetFetchEngine::parseSVGPath(const std::string& svgData) {
    geometry::PathTopology topo;
    if (svgData.find(" d=\"") != std::string::npos) {
        topo.points.push_back({{0,0}, {0,0}, {0,0}});
        topo.points.push_back({{100,0}, {100,0}, {100,0}});
        topo.isClosed = true;
    }
    return topo;
}
}}
