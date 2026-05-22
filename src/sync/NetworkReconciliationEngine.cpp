#include "sync/NetworkReconciliationEngine.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/TextNode.hpp"
#include <cstring>
#include <iostream>

namespace vectma {

NetworkReconciliationEngine::NetworkReconciliationEngine(SceneGraph& scene)
    : m_scene(scene), m_running(false) {}

NetworkReconciliationEngine::~NetworkReconciliationEngine() {
    stop();
}

void NetworkReconciliationEngine::start() {
    if (m_running) return;
    m_running = true;
    m_worker = std::thread(&NetworkReconciliationEngine::workerLoop, this);
}

void NetworkReconciliationEngine::stop() {
    if (!m_running) return;
    m_running = false;
    if (m_worker.joinable()) {
        m_worker.join();
    }
}

void NetworkReconciliationEngine::pushRemoteDelta(const std::vector<uint8_t>& delta) {
    std::lock_guard<std::mutex> lock(m_inMutex);
    m_inboundQueue.push(delta);
}

void NetworkReconciliationEngine::pushLocalDelta(const std::vector<uint8_t>& delta) {
    std::lock_guard<std::mutex> lock(m_outMutex);
    m_outboundQueue.push(delta);
}

bool NetworkReconciliationEngine::popLocalDelta(std::vector<uint8_t>& outDelta) {
    std::lock_guard<std::mutex> lock(m_outMutex);
    if (m_outboundQueue.empty()) return false;
    outDelta = std::move(m_outboundQueue.front());
    m_outboundQueue.pop();
    return true;
}

void NetworkReconciliationEngine::workerLoop() {
    while (m_running) {
        std::vector<uint8_t> delta;
        bool hasDelta = false;

        {
            std::lock_guard<std::mutex> lock(m_inMutex);
            if (!m_inboundQueue.empty()) {
                delta = std::move(m_inboundQueue.front());
                m_inboundQueue.pop();
                hasDelta = true;
            }
        }

        if (hasDelta) {
            try {
                auto msg = DeltaFrameCodec::decode(delta);
                applyDelta(msg);
            } catch (const std::exception& e) {
                std::cerr << "Sync Error: " << e.what() << std::endl;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void NetworkReconciliationEngine::applyDelta(const DeltaFrameCodec::DecodedMessage& msg) {
    LamportClock::getInstance().merge(msg.header.tick);

    if (msg.header.type == MessageType::NodeAdded) {
        const uint8_t* ptr = msg.payload.data();
        uint32_t nameLen;
        std::memcpy(&nameLen, ptr, 4);
        ptr += 4;
        std::string className((const char*)ptr, nameLen);

        std::unique_ptr<CanvasNode> newNode;
        if (className == "RectNode") newNode = std::make_unique<RectNode>(0, 0, 0, 0);
        else if (className == "EllipseNode") newNode = std::make_unique<EllipseNode>(0, 0, 0, 0);
        else if (className == "TextNode") newNode = std::make_unique<TextNode>("");

        if (newNode) {
            newNode->setIdRemote(msg.nodeId, msg.header.tick);
            m_scene.addChildRemote(std::move(newNode), msg.header.tick);
        }
    } else if (msg.header.type == MessageType::NodeUpdated) {
        CanvasNode* node = m_scene.findNodeById(msg.nodeId);
        if (!node) return;

        const uint8_t* ptr = msg.payload.data();
        if (msg.propertyMask & PropertyFlags::Position) {
            double x; std::memcpy(&x, ptr, 8); ptr += 8;
            double y; std::memcpy(&y, ptr, 8); ptr += 8;
            if (auto* r = dynamic_cast<RectNode*>(node)) r->setXRemote(x, msg.header.tick);
            else if (auto* t = dynamic_cast<TextNode*>(node)) { t->setXRemote(x, msg.header.tick); t->setYRemote(y, msg.header.tick); }
        }

        if (msg.propertyMask & PropertyFlags::Color) {
            GColor col; std::memcpy(&col, ptr, sizeof(GColor)); ptr += sizeof(GColor);
            node->setFillColorRemote(col, msg.header.tick);
        }

        if (msg.propertyMask & PropertyFlags::Dimensions) {
            double w; std::memcpy(&w, ptr, 8); ptr += 8;
            double h; std::memcpy(&h, ptr, 8); ptr += 8;
            if (auto* r = dynamic_cast<RectNode*>(node)) { r->setWRemote(w, msg.header.tick); r->setHRemote(h, msg.header.tick); }
        }
    } else if (msg.header.type == MessageType::NodeRemoved) {
        m_scene.removeChildRemote(msg.nodeId, msg.header.tick);
    }
}

} // namespace vectma
