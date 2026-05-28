#include "ui/WorkspaceWindow.hpp"
#include "ui/ThemeTokens.hpp"
#include "ui/LayerListModel.hpp"
#include "core/ArtboardNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include "geometry/PathfinderEngine.hpp"
#include "prototype/InteractionGraph.hpp"
#include <iostream>

namespace vectma {

WorkspaceWindow::WorkspaceWindow(WorkspaceStage& stage, RenderPipeline& renderer)
    : AWindow("Vectma Studio", 1280, 720), m_stage(stage), m_renderer(renderer) {
    m_exportHub = std::make_shared<ExportHub>(m_stage);
    setupLayout();
}

void WorkspaceWindow::setupLayout() {
    m_mainLayout = std::make_shared<aui::AVerticalLayout>();

    // 1. Toolbar
    m_toolbar = std::make_shared<aui::AHorizontalLayout>();
    m_toolbar->setStyle("height: 48px; background: #202020; padding: 4px;");

    auto tools = {"Pointer", "Vector", "Pen", "Text", "Image", "Brush", "Hotspot", "Export"};
    for (const char* name : tools) {
        auto btn = std::make_shared<aui::AButton>(name);
        btn->onClick([this, name]() {
            if (std::string(name) == "Pointer") m_stage.setTool(ToolType::Select);
            else if (std::string(name) == "Pen") m_stage.setTool(ToolType::Pen);
            else if (std::string(name) == "Hotspot") m_stage.setTool(ToolType::Interaction);
            else if (std::string(name) == "Export") m_exportHub->open();
            setupLayout(); // Refresh UI state
        });
        m_toolbar->addView(btn);
    }

    m_mainLayout->addView(m_toolbar);

    // 2. Center Content
    m_centerLayout = std::make_shared<aui::AHorizontalLayout>();

    // Left Panel: Layer Tree
    m_leftPanel = std::make_shared<aui::AVerticalLayout>();
    m_leftPanel->setStyle("width: 240px; background: #1b1b1c; border-right: 1px solid #334155;");

    if (m_stage.getScene()) {
        auto layerModel = std::make_shared<LayerListModel>(m_stage.getScene());
        for (size_t i = 0; i < layerModel->rowCount(); ++i) {
            auto node = layerModel->data(i);
            auto item = std::make_shared<aui::AButton>(node->getClassName());
            item->onClick([this, node]() {
                m_stage.clearSelection();
                m_stage.addToSelection(node);
                setupLayout();
            });
            m_leftPanel->addView(item);
        }
    }

    m_centerLayout->addView(m_leftPanel);

    m_viewport = std::make_shared<aui::AView>();
    m_viewport->setStyle("flex: 1; background: #131313;");
    m_centerLayout->addView(m_viewport);

    // Right Panel: Inspector
    m_rightPanel = std::make_shared<aui::AVerticalLayout>();
    m_rightPanel->setStyle("width: 240px; background: #1b1b1c; border-left: 1px solid #334155; padding: 12px;");

    const auto& selection = m_stage.getSelection();
    if (!selection.empty()) {
        CanvasNode* node = selection[0];

        auto geoLabel = std::make_shared<aui::AButton>("GEOMETRY");
        m_rightPanel->addView(geoLabel);

        auto xField = std::make_shared<aui::ATextField>(std::to_string(node->getX()));
        xField->onChanged([node, this](const std::string& val) {
            try {
                node->setPositionRemote(std::stod(val), node->getY(), LamportClock::getInstance().tick());
            } catch(...) {}
        });
        m_rightPanel->addView(xField);

        // Pathfinder Actions
        if (auto* compound = dynamic_cast<CompoundShapeNode*>(node)) {
            auto flattenBtn = std::make_shared<aui::AButton>("Flatten Paths");
            flattenBtn->onClick([this, compound]() {
                geometry::PathfinderEngine::Flatten(compound);
                m_stage.clearSelection();
                setupLayout();
            });
            m_rightPanel->addView(flattenBtn);
        }

        if (node->getStrokeWidth() > 0) {
            auto outlineBtn = std::make_shared<aui::AButton>("Outline Stroke");
            outlineBtn->onClick([this, node]() {
                geometry::PathfinderEngine::ConvertStrokeToPath(node);
                setupLayout();
            });
            m_rightPanel->addView(outlineBtn);
        }

        auto protoLabel = std::make_shared<aui::AButton>("PROTOTYPING");
        m_rightPanel->addView(protoLabel);

        const auto& links = InteractionGraph::getInstance().getLinksBySource(node->getId());
        for (const auto& link : links) {
             auto linkBtn = std::make_shared<aui::AButton>("Target Artboard: " + std::to_string(link.targetArtboardId.timestamp));
             m_rightPanel->addView(linkBtn);
        }

        if (m_stage.getTool() == ToolType::Interaction) {
            auto hint = std::make_shared<aui::AButton>("Drag wire from handle to link");
            m_rightPanel->addView(hint);
        }
    } else {
        auto noSel = std::make_shared<aui::AButton>("No Selection");
        m_rightPanel->addView(noSel);
    }

    m_centerLayout->addView(m_rightPanel);
    m_mainLayout->addView(m_centerLayout);

    // 3. Status Bar
    m_statusBar = std::make_shared<aui::AHorizontalLayout>();
    m_statusBar->setStyle("height: 24px; background: #131313; padding: 0 10px;");

    Point2D cursor = m_stage.getCursorCanvasPos();
    auto status = std::make_shared<aui::AButton>("X: " + std::to_string((int)cursor.x) + " Y: " + std::to_string((int)cursor.y));
    m_statusBar->addView(status);

    m_mainLayout->addView(m_statusBar);

    setContentView(m_mainLayout);
}

void WorkspaceWindow::show() {
    m_isVisible = true;
}

void WorkspaceWindow::render() {
    if (m_contentView) {
        m_contentView->render();
    }

    m_renderer.beginFrame();
    m_renderer.pushTransform(m_stage.getViewMatrix());
    if (m_stage.getScene()) {
        m_stage.getScene()->render(m_renderer);
    }

    if (m_stage.getTool() == ToolType::Interaction) {
        for (auto* node : m_stage.getSelection()) {
            GRect bbox = node->computeBoundingBox();
            Point2D handle = { bbox.x + bbox.width, bbox.y + bbox.height / 2.0 };
            if (m_stage.isDrawingWire()) {
                m_renderer.drawInteractionWire(handle, m_stage.getWireTargetPos());
            }
        }
    }

    m_renderer.popTransform();
    if (m_exportHub->isOpen()) m_exportHub->render();
    m_renderer.endFrame();
}

} // namespace vectma
