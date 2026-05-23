#include "core/CanvasNode.hpp"

namespace vectma {

void CanvasNode::CloneBaseProperties(const CanvasNode& src, CanvasNode& dst) {
    dst.setVisibility(src.isVisible());
    dst.setLocked(src.isLocked());
    dst.setOpacity(src.getOpacity());
    dst.setBlendMode(src.getBlendMode());
    dst.setFillColor(src.getFillColor());
    dst.setStrokeWidth(src.getStrokeWidth());
}

}
