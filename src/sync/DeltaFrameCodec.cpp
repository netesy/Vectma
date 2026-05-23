#include "sync/DeltaFrameCodec.hpp"
namespace vectma {
std::vector<uint8_t> DeltaFrameCodec::encodeUpdate(const CanvasNode&, uint32_t, LamportTimestamp) { return {}; }
std::vector<uint8_t> DeltaFrameCodec::encodeAddition(const CanvasNode&, LamportTimestamp) { return {}; }
std::vector<uint8_t> DeltaFrameCodec::encodeRemoval(NodeId, LamportTimestamp) { return {}; }
DeltaFrameCodec::DecodedMessage DeltaFrameCodec::decode(const std::vector<uint8_t>&) { return {}; }
}
