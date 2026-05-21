#include "ui/ExportDashboard.hpp"
#include "ui/ThemeTokens.hpp"
#include "core/VectorExporter.hpp"
#include "core/ImageEngine.hpp"
#include "renderer/RenderPipeline.hpp"
#include <imgui.h>
#include <fstream>

namespace vectma {

void ExportDashboard::render(WorkspaceStage& stage, RenderPipeline& renderer, bool& showExport) {
    if (!showExport) return;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Export & Handoff Hub", &showExport)) {

        ImGui::Text("EXPORT SETTINGS");
        ImGui::Separator();

        static int formatIdx = 1;
        const char* formats[] = { "PNG", "SVG", "PDF" };
        ImGui::Combo("Format", &formatIdx, formats, 3);

        static float scale = 1.0f;
        ImGui::SliderFloat("Scale", &scale, 1.0f, 4.0f, "%.0fx");

        if (ImGui::Button("Export to File", ImVec2(200, 40))) {
            auto scene = stage.getScene();
            if (scene) {
                if (formatIdx == 0) {
                    auto data = renderer.exportRaster(scale);
                    if (!data.empty()) {
                        std::ofstream f("export.png", std::ios::binary);
                        f.write((char*)data.data(), data.size());
                    }
                } else if (formatIdx == 1) {
                    VectorExporter::ExportToSVG("export.svg", *scene);
                } else if (formatIdx == 2) {
                    VectorExporter::ExportToPDF("export.pdf", *scene);
                }
            }
            showExport = false;
        }

        ImGui::Separator();
        ImGui::Text("ADVANCED");

        static bool includeMetadata = true;
        ImGui::Checkbox("Include Metadata", &includeMetadata);

        static bool optimizePaths = true;
        ImGui::Checkbox("Optimize Paths", &optimizePaths);

        ImGui::End();
    }
}

} // namespace vectma
