#include "renderer/RenderPipeline.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include <GL/glew.h>
#include <iostream>
#include <cmath>

namespace vectma {

void BaselineRenderer::beginFrame() {
    // Basic GL setup for the frame
}

void BaselineRenderer::endFrame() {
    // Finalize GL frame
}

void BaselineRenderer::drawRect(const RectNode& node) {
    glColor3f(0.0f, 0.5f, 0.5f); // Teal
    glBegin(GL_LINE_LOOP);
    double x = node.getX();
    double y = node.getY();
    double w = node.getW();
    double h = node.getH();
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void BaselineRenderer::drawEllipse(const EllipseNode& node) {
    glColor3f(0.0f, 0.7f, 0.7f); // Lighter Teal
    glBegin(GL_LINE_LOOP);
    double cx = node.getCX();
    double cy = node.getCY();
    double rx = node.getRX();
    double ry = node.getRY();
    for (int i = 0; i < 360; i += 10) {
        double rad = i * M_PI / 180.0;
        glVertex2f(cx + cos(rad) * rx, cy + sin(rad) * ry);
    }
    glEnd();
}

void BaselineRenderer::drawPath(const PathNode& node) {
    (void)node;
    // Path rendering placeholder
}

void BaselineRenderer::renderNode(const CanvasNode& node) {
    (void)node;
}

} // namespace vectma
