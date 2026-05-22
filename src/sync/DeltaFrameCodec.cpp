#include "sync/DeltaFrameCodec.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/TextNode.hpp"
#include <cstring>
#include <stdexcept>

namespace vectma {

template <typename T>
void writePOD(std::vector<uint8_t>& buf, const T& val) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&val);
    buf.insert(buf.end(), ptr, ptr + sizeof(T));
}

template <typename T>
T readPOD(const uint8_t*& ptr) {
    T val;
    std::memcpy(&val, ptr, sizeof(T));
    ptr += sizeof(T);
    return val;
}

std::vector<uint8_t> DeltaFrameCodec::encodeUpdate(const CanvasNode& node, uint32_t propertyMask, LamportTimestamp ts) {
    std::vector<uint8_t> buffer;
    writePOD(buffer, MessageType::NodeUpdated);
    writePOD(buffer, LamportClock::getInstance().getClientId());
    writePOD(buffer, ts);
    writePOD(buffer, node.getId());
    writePOD(buffer, propertyMask);

    if (propertyMask & PropertyFlags::Position) {
        if (auto* rect = dynamic_cast<const RectNode*>(&node)) {
            writePOD(buffer, rect->getX());
            writePOD(buffer, rect->getY());
        } else if (auto* text = dynamic_cast<const TextNode*>(&node)) {
            writePOD(buffer, text->getX());
            writePOD(buffer, text->getY());
        }
    }

    if (propertyMask & PropertyFlags::Color) {
        writePOD(buffer, node.getFillColor());
    }

    if (propertyMask & PropertyFlags::Dimensions) {
        if (auto* rect = dynamic_cast<const RectNode*>(&node)) {
            writePOD(buffer, rect->getW());
            writePOD(buffer, rect->getH());
        }
    }

    return buffer;
}

std::vector<uint8_t> DeltaFrameCodec::encodeAddition(const CanvasNode& node, LamportTimestamp ts) {
    std::vector<uint8_t> buffer;
    writePOD(buffer, MessageType::NodeAdded);
    writePOD(buffer, LamportClock::getInstance().getClientId());
    writePOD(buffer, ts);
    writePOD(buffer, node.getId());

    std::string className = node.getClassName();
    uint32_t nameLen = className.length();
    writePOD(buffer, nameLen);
    buffer.insert(buffer.end(), className.begin(), className.end());

    return buffer;
}

std::vector<uint8_t> DeltaFrameCodec::encodeRemoval(NodeId id, LamportTimestamp ts) {
    std::vector<uint8_t> buffer;
    writePOD(buffer, MessageType::NodeRemoved);
    writePOD(buffer, LamportClock::getInstance().getClientId());
    writePOD(buffer, ts);
    writePOD(buffer, id);
    return buffer;
}

DeltaFrameCodec::DecodedMessage DeltaFrameCodec::decode(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < sizeof(MessageType) + sizeof(ClientId) + sizeof(LamportTimestamp)) {
        throw std::runtime_error("Buffer too small");
    }

    const uint8_t* ptr = buffer.data();
    DecodedMessage msg;
    msg.header.type = readPOD<MessageType>(ptr);
    msg.header.originClientId = readPOD<ClientId>(ptr);
    msg.header.tick = readPOD<LamportTimestamp>(ptr);
    msg.nodeId = readPOD<NodeId>(ptr);

    if (msg.header.type == MessageType::NodeUpdated) {
        msg.propertyMask = readPOD<uint32_t>(ptr);
    }

    size_t payloadStart = ptr - buffer.data();
    if (payloadStart < buffer.size()) {
        msg.payload.assign(ptr, buffer.data() + buffer.size());
    }

    return msg;
}

} // namespace vectma
