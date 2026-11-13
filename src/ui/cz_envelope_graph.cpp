#include "cz_envelope_graph.h"
#include "cz_tutorial.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace cz {

CzEnvelopeGraph::CzEnvelopeGraph() {
}

bool CzEnvelopeGraph::render(CzPatch& patch, int active_line, float width, float height) {
    bool modified = false;

    CzLine& line = (active_line == 1) ? patch.line1 : patch.line2;

    // Controls toolbar
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "GRAFICO DE ENVOLVENTES (8 PASOS):");
    ImGui::SameLine();

    // Section selector buttons
    if (ImGui::RadioButton("DCO (Tono)", active_section_ == 0)) active_section_ = 0;
    CzTutorial::instance().tooltip("dco");
    ImGui::SameLine();

    if (ImGui::RadioButton("DCW (Brillo/Filtro PD)", active_section_ == 1)) active_section_ = 1;
    CzTutorial::instance().tooltip("dcw");
    ImGui::SameLine();

    if (ImGui::RadioButton("DCA (Volumen)", active_section_ == 2)) active_section_ = 2;
    CzTutorial::instance().tooltip("dca");
    ImGui::SameLine();

    if (ImGui::RadioButton("TODAS (Superpuestas)", active_section_ == 3)) active_section_ = 3;
    CzTutorial::instance().tooltip("envelope");

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImVec2(width, height);

    // Canvas Background
    draw_list->AddRectFilled(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(20, 24, 28, 255), 4.0f);
    draw_list->AddRect(canvas_pos, ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y), IM_COL32(50, 60, 70, 255), 4.0f);

    // Margins
    const float pad_left = 35.0f;
    const float pad_right = 20.0f;
    const float pad_top = 15.0f;
    const float pad_bottom = 25.0f;

    const float plot_w = canvas_size.x - pad_left - pad_right;
    const float plot_h = canvas_size.y - pad_top - pad_bottom;

    // Draw grid lines
    // Y-axis grid (0%, 25%, 50%, 75%, 100%)
    for (int i = 0; i <= 4; ++i) {
        float y = canvas_pos.y + pad_top + (plot_h * (1.0f - (i * 0.25f)));
        draw_list->AddLine(ImVec2(canvas_pos.x + pad_left, y), ImVec2(canvas_pos.x + pad_left + plot_w, y), IM_COL32(35, 42, 50, 255), 1.0f);

        char val_buf[8];
        snprintf(val_buf, sizeof(val_buf), "%d", i * 25);
        draw_list->AddText(ImVec2(canvas_pos.x + 8.0f, y - 6.0f), IM_COL32(110, 125, 140, 255), val_buf);
    }

    // X-axis step grid (8 steps)
    for (int step = 1; step <= 8; ++step) {
        float x = canvas_pos.x + pad_left + ((step / 8.0f) * plot_w);
        draw_list->AddLine(ImVec2(x, canvas_pos.y + pad_top), ImVec2(x, canvas_pos.y + pad_top + plot_h), IM_COL32(35, 42, 50, 255), 1.0f);

        char step_buf[8];
        snprintf(step_buf, sizeof(step_buf), "S%d", step);
        draw_list->AddText(ImVec2(x - 8.0f, canvas_pos.y + pad_top + plot_h + 5.0f), IM_COL32(110, 125, 140, 255), step_buf);
    }

    // Lambda to plot an envelope
    auto draw_env = [&](const CzEnvelope& env, ImU32 color, bool is_interactive, const char* name) {
        std::vector<ImVec2> pts;
        // Start point: Step 0 is at Level 0
        pts.push_back(ImVec2(canvas_pos.x + pad_left, canvas_pos.y + pad_top + plot_h));

        for (int i = 0; i < env.end_step; ++i) {
            float x = canvas_pos.x + pad_left + (((i + 1) / 8.0f) * plot_w);
            float norm_lvl = std::clamp(static_cast<float>(env.steps[i].level) / 99.0f, 0.0f, 1.0f);
            float y = canvas_pos.y + pad_top + (plot_h * (1.0f - norm_lvl));
            pts.push_back(ImVec2(x, y));
        }

        // Draw connecting lines
        for (size_t i = 1; i < pts.size(); ++i) {
            draw_list->AddLine(pts[i - 1], pts[i], color, is_interactive ? 2.5f : 1.5f);
        }

        // Draw nodes and handles
        for (size_t i = 1; i < pts.size(); ++i) {
            int step_idx = static_cast<int>(i);
            bool is_selected = (is_interactive && selected_step_ == step_idx);
            float r = is_selected ? 6.0f : 4.0f;

            draw_list->AddCircleFilled(pts[i], r, color);
            draw_list->AddCircle(pts[i], r + 1.0f, IM_COL32(0, 0, 0, 200), 0, 1.5f);

            if (is_selected) {
                draw_list->AddCircle(pts[i], r + 3.0f, IM_COL32(255, 255, 255, 220), 0, 1.5f);
            }
        }

        // Draw Sustain marker
        if (env.sustain_step > 0 && env.sustain_step <= env.end_step) {
            float sus_x = canvas_pos.x + pad_left + ((env.sustain_step / 8.0f) * plot_w);
            draw_list->AddLine(
                ImVec2(sus_x, canvas_pos.y + pad_top),
                ImVec2(sus_x, canvas_pos.y + pad_top + plot_h),
                IM_COL32(255, 215, 0, 160), 1.5f
            );
            draw_list->AddText(ImVec2(sus_x - 12.0f, canvas_pos.y + pad_top - 12.0f), IM_COL32(255, 215, 0, 240), "SUS");
        }

        // Draw End marker
        if (env.end_step > 0 && env.end_step <= 8) {
            float end_x = canvas_pos.x + pad_left + ((env.end_step / 8.0f) * plot_w);
            draw_list->AddLine(
                ImVec2(end_x, canvas_pos.y + pad_top),
                ImVec2(end_x, canvas_pos.y + pad_top + plot_h),
                IM_COL32(255, 60, 60, 160), 1.5f
            );
            draw_list->AddText(ImVec2(end_x - 10.0f, canvas_pos.y + pad_top - 12.0f), IM_COL32(255, 60, 60, 240), "END");
        }
    };

    // Render layers
    if (active_section_ == 3) { // Layered view
        draw_env(line.dco, IM_COL32(32, 210, 255, 180), false, "DCO");
        draw_env(line.dcw, IM_COL32(255, 175, 32, 220), true, "DCW");
        draw_env(line.dca, IM_COL32(64, 255, 96, 180), false, "DCA");
    } else if (active_section_ == 0) {
        draw_env(line.dco, IM_COL32(32, 210, 255, 255), true, "DCO");
    } else if (active_section_ == 1) {
        draw_env(line.dcw, IM_COL32(255, 175, 32, 255), true, "DCW");
    } else if (active_section_ == 2) {
        draw_env(line.dca, IM_COL32(64, 255, 96, 255), true, "DCA");
    }

    // Advance layout
    ImGui::Dummy(canvas_size);

    // Envelope step editor controls below the graph
    CzEnvelope* active_env = nullptr;
    const char* env_name = "";
    if (active_section_ == 0) { active_env = &line.dco; env_name = "DCO (PITCH)"; }
    else if (active_section_ == 1 || active_section_ == 3) { active_env = &line.dcw; env_name = "DCW (WAVEFORM / PD)"; }
    else if (active_section_ == 2) { active_env = &line.dca; env_name = "DCA (AMPLITUDE)"; }

    if (active_env) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "EDITANDO: %s", env_name);
        ImGui::SameLine();

        // Step selector buttons 1..8
        ImGui::Text("  Paso:");
        ImGui::PushID("EnvSteps");
        for (int s = 1; s <= 8; ++s) {
            ImGui::SameLine();
            char btn_id[16];
            snprintf(btn_id, sizeof(btn_id), "%d##step", s);
            if (s == selected_step_) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.9f, 1.0f));
            }
            if (ImGui::Button(btn_id, ImVec2(24, 22))) {
                selected_step_ = s;
            }
            if (s == selected_step_) {
                ImGui::PopStyleColor();
            }
        }
        ImGui::PopID();

        // Active step sliders
        int step_idx = selected_step_ - 1;
        int rate_val = active_env->steps[step_idx].rate;
        int lvl_val = active_env->steps[step_idx].level;

        ImGui::SetNextItemWidth(140);
        if (ImGui::SliderInt("Rate (Velocidad)", &rate_val, 0, 99)) {
            active_env->steps[step_idx].rate = static_cast<uint8_t>(rate_val);
            modified = true;
        }
        CzTutorial::instance().tooltip("envelope");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(140);
        if (ImGui::SliderInt("Level (Nivel)", &lvl_val, 0, 99)) {
            active_env->steps[step_idx].level = static_cast<uint8_t>(lvl_val);
            modified = true;
        }
        CzTutorial::instance().tooltip("envelope");

        ImGui::SameLine();
        // Sustain toggle
        bool is_sus = (active_env->sustain_step == selected_step_);
        if (ImGui::Checkbox("Sustain (SUS)", &is_sus)) {
            active_env->sustain_step = is_sus ? selected_step_ : 0;
            modified = true;
        }
        CzTutorial::instance().tooltip("sustain");

        ImGui::SameLine();
        // End toggle
        bool is_end = (active_env->end_step == selected_step_);
        if (ImGui::Checkbox("End (FIN)", &is_end)) {
            active_env->end_step = is_end ? selected_step_ : 8;
            modified = true;
        }
        CzTutorial::instance().tooltip("end_step");
    }

    return modified;
}

} // namespace cz

