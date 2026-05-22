#pragma once

#include "core/SceneGraph.hpp"
#include "sync/DeltaFrameCodec.hpp"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace vectma {

class NetworkReconciliationEngine {
public:
    NetworkReconciliationEngine(SceneGraph& scene);
    ~NetworkReconciliationEngine();

    void pushRemoteDelta(const std::vector<uint8_t>& delta);
    void pushLocalDelta(const std::vector<uint8_t>& delta);

    bool popLocalDelta(std::vector<uint8_t>& outDelta);

    void start();
    void stop();

private:
    SceneGraph& m_scene;
    std::atomic<bool> m_running;
    std::thread m_worker;

    std::queue<std::vector<uint8_t>> m_inboundQueue;
    std::queue<std::vector<uint8_t>> m_outboundQueue;
    std::mutex m_inMutex;
    std::mutex m_outMutex;

    void workerLoop();
    void applyDelta(const DeltaFrameCodec::DecodedMessage& msg);
};

} // namespace vectma
