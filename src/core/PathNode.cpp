#include "core/PathNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include <cmath>
#include <algorithm>
#include <map>
#include <set>

namespace vectma {

PathNode::PathNode() {
    LamportTimestamp ts{0, 0, 0};
    m_id.update({0, 0, 0}, ts);
}

PathNode::PathNode(const std::vector<BezierAnchor>& anchors) {
    LamportTimestamp ts{0, 0, 0};
    m_id.update({0, 0, 0}, ts);
    for (size_t i = 0; i < anchors.size(); ++i) {
        geometry::AnchorPoint ap;
        ap.position = anchors[i].position;
        ap.handleInOffset = {anchors[i].handleIn.x - ap.position.x, anchors[i].handleIn.y - ap.position.y};
        ap.handleOutOffset = {anchors[i].handleOut.x - ap.position.x, anchors[i].handleOut.y - ap.position.y};
        m_topology.points.push_back(ap);
        if (i > 0) m_topology.edges.push_back({i - 1, i});
    }
}

PathNode::PathNode(const geometry::PathTopology& topology) : m_topology(topology) {
    LamportTimestamp ts{0, 0, 0};
    m_id.update({0, 0, 0}, ts);
}

void PathNode::setTopology(const geometry::PathTopology& topology) {
    m_topology = topology;
    m_geometry_dirty = true;
}

void PathNode::addAnchor(const geometry::AnchorPoint& point) {
    size_t prevIdx = m_topology.points.empty() ? 0 : m_topology.points.size() - 1;
    m_topology.points.push_back(point);
    if (m_topology.points.size() > 1) {
        m_topology.edges.push_back({prevIdx, m_topology.points.size() - 1});
    }
    m_geometry_dirty = true;
}

void PathNode::updateAnchor(size_t index, const geometry::AnchorPoint& point) {
    if (index < m_topology.points.size()) {
        m_topology.points[index] = point;
        m_geometry_dirty = true;
    }
}

void PathNode::setClosed(bool closed) {
    if (m_topology.isClosed != closed) {
        m_topology.isClosed = closed;
        if (closed && m_topology.points.size() > 2) {
             // Check if already closed
             bool exists = false;
             for(const auto& e : m_topology.edges) {
                 if((e.fromIdx == m_topology.points.size()-1 && e.toIdx == 0) ||
                    (e.fromIdx == 0 && e.toIdx == m_topology.points.size()-1)) {
                     exists = true; break;
                 }
             }
             if(!exists) m_topology.edges.push_back({m_topology.points.size() - 1, 0});
        }
        m_geometry_dirty = true;
    }
}

const std::vector<BezierAnchor>& PathNode::getAnchors() const {
    if (m_geometry_dirty) {
        m_legacy_anchors_cache.clear();
        for (const auto& p : m_topology.points) {
            m_legacy_anchors_cache.emplace_back(p.position, p.getHandleIn(), p.getHandleOut());
        }
    }
    return m_legacy_anchors_cache;
}

void PathNode::setAnchors(const std::vector<BezierAnchor>& anchors) {
    m_topology.points.clear();
    m_topology.edges.clear();
    for (size_t i = 0; i < anchors.size(); ++i) {
        geometry::AnchorPoint ap;
        ap.position = anchors[i].position;
        ap.handleInOffset = {anchors[i].handleIn.x - ap.position.x, anchors[i].handleIn.y - ap.position.y};
        ap.handleOutOffset = {anchors[i].handleOut.x - ap.position.x, anchors[i].handleOut.y - ap.position.y};
        m_topology.points.push_back(ap);
        if (i > 0) m_topology.edges.push_back({i - 1, i});
    }
    m_geometry_dirty = true;
}

void PathNode::addModifier(std::unique_ptr<Modifier> modifier) {
    m_modifier_stack.push_back(std::move(modifier));
}

void PathNode::updatePipelineCache() const {
    bool stack_dirty = false;
    for (const auto& mod : m_modifier_stack) {
        if (mod->isDirty()) { stack_dirty = true; break; }
    }

    if (!m_geometry_dirty && !stack_dirty && m_cached_compiled_path) return;

    auto current_data = std::make_unique<PathData>();

    // Greedy edge joiner to form contours
    std::vector<bool> used(m_topology.edges.size(), false);
    for (size_t i = 0; i < m_topology.edges.size(); ++i) {
        if (used[i]) continue;

        std::vector<BezierAnchor> contourAnchors;
        size_t currEdge = i;
        used[currEdge] = true;

        size_t startNode = m_topology.edges[currEdge].fromIdx;
        size_t endNode = m_topology.edges[currEdge].toIdx;

        auto& pStart = m_topology.points[startNode];
        auto& pEnd = m_topology.points[endNode];

        contourAnchors.emplace_back(pStart.position, pStart.getHandleIn(), m_topology.edges[currEdge].fromIdx == startNode ? pStart.getHandleOut() : pStart.getHandleIn());
        contourAnchors.emplace_back(pEnd.position, m_topology.edges[currEdge].toIdx == endNode ? pEnd.getHandleIn() : pEnd.getHandleOut(), pEnd.getHandleOut());

        // Try to grow forward
        bool growing = true;
        while(growing) {
            growing = false;
            for(size_t j = 0; j < m_topology.edges.size(); ++j) {
                if(used[j]) continue;
                if(m_topology.edges[j].fromIdx == endNode) {
                    endNode = m_topology.edges[j].toIdx;
                    auto& p = m_topology.points[endNode];
                    contourAnchors.back().handleOut = m_topology.points[m_topology.edges[j].fromIdx].getHandleOut();
                    contourAnchors.emplace_back(p.position, p.getHandleIn(), p.getHandleOut());
                    used[j] = true;
                    growing = true;
                    break;
                }
            }
        }

        bool closed = (endNode == startNode);
        if(closed && contourAnchors.size() > 1) {
            contourAnchors.pop_back(); // Remove redundant last point
        }

        current_data->contours.emplace_back(contourAnchors, closed);
    }

    for (const auto& mod : m_modifier_stack) {
        current_data = mod->apply(*current_data);
    }

    m_cached_compiled_path = std::move(current_data);
    m_geometry_dirty = false;
}

const PathData& PathNode::getCompiledPath() const {
    updatePipelineCache();
    return *m_cached_compiled_path;
}

void PathNode::render(RenderPipeline& pipeline) const {
    updatePipelineCache();
    pipeline.drawPath(*this, getFillType(), getGradientConfig(), getStrokeAlignment());
}

bool PathNode::containsPoint(const GPoint& point) const {
    for (const auto& edge : m_topology.edges) {
        double t;
        const auto& p1 = m_topology.points[edge.fromIdx];
        const auto& p2 = m_topology.points[edge.toIdx];
        double dist = geometry::BezierEvaluator::distanceToCubic(
            p1.position, p1.getHandleOut(), p2.getHandleIn(), p2.position,
            point, t
        );
        if (dist < 4.0) return true;
    }
    for (const auto& p : m_topology.points) {
        double dx = p.position.x - point.x;
        double dy = p.position.y - point.y;
        if (std::sqrt(dx*dx + dy*dy) < 6.0) return true;
    }
    return false;
}

GRect PathNode::computeBoundingBox() const {
    if (m_topology.points.empty()) return GRect(0, 0, 0, 0);
    GRect bbox = GRect(m_topology.points[0].position.x, m_topology.points[0].position.y, 0, 0);
    for (const auto& edge : m_topology.edges) {
        const auto& p1 = m_topology.points[edge.fromIdx];
        const auto& p2 = m_topology.points[edge.toIdx];
        bbox = bbox.united(geometry::BezierEvaluator::cubicBounds(
            p1.position, p1.getHandleOut(), p2.getHandleIn(), p2.position
        ));
    }
    double sw = getStrokeWidth();
    bbox.x -= sw; bbox.y -= sw;
    bbox.width += sw * 2; bbox.height += sw * 2;
    return bbox;
}

std::unique_ptr<CanvasNode> PathNode::clone() const {
    auto copy = std::make_unique<PathNode>(m_topology);
    CanvasNode::CloneBaseProperties(*this, *copy);
    return copy;
}
std::string PathNode::toSVG() const { return "<path d=\"...\" />"; }

void PathNode::setBaseContours(const std::vector<Contour>& contours) {
    m_topology.points.clear();
    m_topology.edges.clear();
    for (const auto& contour : contours) {
        size_t startIdx = m_topology.points.size();
        for (size_t i = 0; i < contour.anchors.size(); ++i) {
            geometry::AnchorPoint ap;
            ap.position = contour.anchors[i].position;
            ap.handleInOffset = {contour.anchors[i].handleIn.x - ap.position.x, contour.anchors[i].handleIn.y - ap.position.y};
            ap.handleOutOffset = {contour.anchors[i].handleOut.x - ap.position.x, contour.anchors[i].handleOut.y - ap.position.y};
            m_topology.points.push_back(ap);
            if (i > 0) m_topology.edges.push_back({startIdx + i - 1, startIdx + i});
        }
        if (contour.isClosed && contour.anchors.size() > 1) {
            m_topology.edges.push_back({m_topology.points.size() - 1, startIdx});
        }
    }
    m_geometry_dirty = true;
}

} // namespace vectma
