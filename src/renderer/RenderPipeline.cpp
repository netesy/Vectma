#include "renderer/RenderPipeline.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"

#ifdef VECTMA_USE_OPENGL
#include <GL/glew.h>
#endif

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
#ifdef VECTMA_USE_OPENGL
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
#else
    (void)node;
#endif
}

void BaselineRenderer::drawEllipse(const EllipseNode& node) {
#ifdef VECTMA_USE_OPENGL
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
#else
    (void)node;
#endif
}

void BaselineRenderer::drawPath(const PathNode& node) {
    drawBezierPath(node);
}

void BaselineRenderer::drawBezierPath(const PathNode& node) {
    const auto& anchors = node.getAnchors();
    if (anchors.size() < 2) return;

#ifdef VECTMA_USE_OPENGL
    glColor3f(0.8f, 0.8f, 0.0f); // Yellowish
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < anchors.size() - 1; ++i) {
        const auto& p0 = anchors[i].position;
        const auto& p1 = anchors[i].handleOut;
        const auto& p2 = anchors[i+1].handleIn;
        const auto& p3 = anchors[i+1].position;

        // Simple cubic bezier subdivision
        for (int t_step = 0; t_step <= 20; ++t_step) {
            double t = t_step / 20.0;
            double t1 = 1.0 - t;
            double x = t1*t1*t1*p0.x + 3*t1*t1*t*p1.x + 3*t1*t*t*p2.x + t*t*t*p3.x;
            double y = t1*t1*t1*p0.y + 3*t1*t1*t*p1.y + 3*t1*t*t*p2.y + t*t*t*p3.y;
            glVertex2f(x, y);
        }
    }
    glEnd();
#else
    (void)node;
#endif
}

void BaselineRenderer::drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) {
#ifdef VECTMA_USE_OPENGL
    auto drawSquare = [](const Point2D& p, float size, bool filled) {
        if (filled) glBegin(GL_QUADS);
        else glBegin(GL_LINE_LOOP);
        glVertex2f(p.x - size, p.y - size);
        glVertex2f(p.x + size, p.y - size);
        glVertex2f(p.x + size, p.y + size);
        glVertex2f(p.x - size, p.y + size);
        glEnd();
    };

    auto drawCircle = [](const Point2D& p, float radius) {
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 360; i += 30) {
            double rad = i * M_PI / 180.0;
            glVertex2f(p.x + cos(rad) * radius, p.y + sin(rad) * radius);
        }
        glEnd();
    };

    // Draw handles first
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(anchor.position.x, anchor.position.y);
    glVertex2f(anchor.handleIn.x, anchor.handleIn.y);
    glVertex2f(anchor.position.x, anchor.position.y);
    glVertex2f(anchor.handleOut.x, anchor.handleOut.y);
    glEnd();

    if (activeHandle == 1) glColor3f(1.0f, 1.0f, 1.0f); else glColor3f(0.7f, 0.7f, 0.7f);
    drawCircle(anchor.handleIn, 3.0f);

    if (activeHandle == 2) glColor3f(1.0f, 1.0f, 1.0f); else glColor3f(0.7f, 0.7f, 0.7f);
    drawCircle(anchor.handleOut, 3.0f);

    // Draw anchor
    if (selected) glColor3f(0.2f, 0.6f, 1.0f);
    else glColor3f(1.0f, 1.0f, 1.0f);

    drawSquare(anchor.position, 4.0f, activeHandle == 0);
#else
    (void)anchor; (void)selected; (void)activeHandle;
#endif
}

void BaselineRenderer::renderNode(const CanvasNode& node) {
    (void)node;
}

} // namespace vectma
