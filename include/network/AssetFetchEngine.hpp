#pragma once
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>
#include "geometry/BezierMath.hpp"
namespace vectma { namespace network {
struct AssetInfo { std::string name, slug, category; };
class AssetFetchEngine {
public:
    static AssetFetchEngine& getInstance() { static AssetFetchEngine instance; return instance; }
    using IndexCallback = std::function<void(const std::vector<AssetInfo>&)>;
    using SVGCallback = std::function<void(const std::string&, const geometry::PathTopology&)>;
    void fetchIndex(IndexCallback cb);
    void fetchSVG(const std::string& slug, SVGCallback cb);
    void stop();
private:
    AssetFetchEngine();
    ~AssetFetchEngine();
    void workerLoop();
    std::string performGet(const std::string& url);
    geometry::PathTopology parseSVGPath(const std::string& data);
    std::thread m_worker;
    std::atomic<bool> m_running;
    std::mutex m_queueMutex;
    struct Task { enum { FetchIndex, FetchSVG } type; std::string slug; IndexCallback indexCb; SVGCallback svgCb; };
    std::queue<Task> m_tasks;
    std::condition_variable m_cv;
};
}}
