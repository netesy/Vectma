#pragma once

#include <vector>
#include <cstdint>
#include "core/CanvasNode.hpp"

namespace vectma {

enum class MessageType : uint8_t {
    NodeAdded = 0,
    NodeUpdated = 1,
    NodeRemoved = 2
};

struct MessageHeader {
    MessageType type;
    ClientId originClientId;
    LamportTimestamp tick;
};

class DeltaFrameCodec {
public:
    static std::vector<uint8_t> encodeUpdate(const CanvasNode& node, uint32_t propertyMask, LamportTimestamp ts);
    static std::vector<uint8_t> encodeAddition(const CanvasNode& node, LamportTimestamp ts);
    static std::vector<uint8_t> encodeRemoval(NodeId id, LamportTimestamp ts);

    struct DecodedMessage {
        MessageHeader header;
        NodeId nodeId;
        uint32_t propertyMask;
        std::vector<uint8_t> payload;
    };

    static DecodedMessage decode(const std::vector<uint8_t>& buffer);
};

namespace PropertyFlags {
    const uint32_t Position    = 1 << 0;
    const uint32_t Color       = 1 << 1;
    const uint32_t Visibility  = 1 << 2;
    const uint32_t Opacity     = 1 << 3;
    const uint32_t Dimensions  = 1 << 4;
    const uint32_t Typography  = 1 << 5;
}

} // namespace vectma
