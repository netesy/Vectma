#include "sync/NetworkReconciliationEngine.hpp"
namespace vectma {
NetworkReconciliationEngine::NetworkReconciliationEngine(SceneGraph& scene) : m_scene(scene), m_running(false) {}
NetworkReconciliationEngine::~NetworkReconciliationEngine() {}
void NetworkReconciliationEngine::pushRemoteDelta(const std::vector<uint8_t>&) {}
void NetworkReconciliationEngine::pushLocalDelta(const std::vector<uint8_t>&) {}
bool NetworkReconciliationEngine::popLocalDelta(std::vector<uint8_t>&) { return false; }
void NetworkReconciliationEngine::start() {}
void NetworkReconciliationEngine::stop() {}
void NetworkReconciliationEngine::workerLoop() {}
void NetworkReconciliationEngine::applyDelta(const DeltaFrameCodec::DecodedMessage&) {}
}
