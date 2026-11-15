#include "cz_envelope_graph.h"
#include "cz_tutorial.h"
#include "../core/cz_i18n.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace cz {

CzEnvelopeGraph::CzEnvelopeGraph() {
}

bool CzEnvelopeGraph::render(CzPatch& patch, int active_line, float width, float height) {
    bool modified = false;

    CzLine& line = (active_line == 1) ? patch.line1 : patch.line2;

    // Controls toolbar (compact padding so all 4 radios stay on 1 row at 590px, wrapping only if needed)
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 3.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.0f, 1.0f));
    const float row_start_x = ImGui::GetCursorScreenPos().x;
    auto same_line_if_fits = [&](float next_est_w) {
        float used_w = ImGui::GetItemRectMax().x - row_start_x;
        if (used_w + next_est_w < width) {
            ImGui::SameLine();
        }
    };

    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "%s", tr("env.title"));
    same_line_if_fits(ImGui::CalcTextSize(tr("env.dco")).x + 24.0f);
    if (ImGui::RadioButton(tr("env.dco"), active_section_ == 0)) active_section_ = 0;
    CzTutorial::instance().tooltip("dco");

    same_line_if_fits(ImGui::CalcTextSize(tr("env.dcw")).x + 24.0f);
    if (ImGui::RadioButton(tr("env.dcw"), active_section_ == 1)) active_section_ = 1;
    CzTutorial::instance().tooltip("dcw");

    same_line_if_fits(ImGui::CalcTextSize(tr("env.dca")).x + 24.0f);
    if (ImGui::RadioButton(tr("env.dca"), active_section_ == 2)) active_section_ = 2;
    CzTutorial::instance().tooltip("dca");

    same_line_if_fits(ImGui::CalcTextSize(tr("env.all")).x + 24.0f);
    if (ImGui::RadioButton(tr("env.all"), active_section_ == 3)) active_section_ = 3;
    CzTutorial::instance().tooltip("envelope");
    ImGui::PopStyleVar(2);

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
        ImVec2 sep_pos = ImGui::GetCursorScreenPos();
        draw_list->AddLine(ImVec2(sep_pos.x, sep_pos.y + 3.0f),
                           ImVec2(sep_pos.x + width, sep_pos.y + 3.0f),
                           IM_COL32(60, 68, 78, 255), 1.0f);
        ImGui::Dummy(ImVec2(width, 7.0f));
        ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), tr("env.editing"), env_name);
        ImGui::SameLine();

        // Step selector buttons 1..8
        ImGui::Text("%s", tr("env.step"));
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

        const float ctrl_row_start_x = ImGui::GetCursorScreenPos().x;
        const float spacing_x = ImGui::GetStyle().ItemSpacing.x;
        const float inner_spacing_x = ImGui::GetStyle().ItemInnerSpacing.x;
        const float frame_h = ImGui::GetFrameHeight();
        auto same_line_if_fits_ctrl = [&](float next_w) {
            if (ImGui::GetItemRectMax().x + spacing_x + next_w <= ctrl_row_start_x + width) {
                ImGui::SameLine();
            }
        };

        float slider_w = (width <= 620.0f) ? 90.0f : 140.0f;
        ImGui::SetNextItemWidth(slider_w);
        if (ImGui::SliderInt(tr("env.rate"), &rate_val, 0, 99)) {
            active_env->steps[step_idx].rate = static_cast<uint8_t>(rate_val);
            modified = true;
        }
        CzTutorial::instance().tooltip("envelope");

        const char* lvl_label = tr("env.level");
        const float lvl_w = slider_w + inner_spacing_x + ImGui::CalcTextSize(lvl_label, nullptr, true).x;
        same_line_if_fits_ctrl(lvl_w);
        ImGui::SetNextItemWidth(slider_w);
        if (ImGui::SliderInt(lvl_label, &lvl_val, 0, 99)) {
            active_env->steps[step_idx].level = static_cast<uint8_t>(lvl_val);
            modified = true;
        }
        CzTutorial::instance().tooltip("envelope");

        const char* sus_label = tr("env.sustain");
        const float sus_w = frame_h + inner_spacing_x + ImGui::CalcTextSize(sus_label, nullptr, true).x;
        same_line_if_fits_ctrl(sus_w);
        // Sustain toggle
        bool is_sus = (active_env->sustain_step == selected_step_);
        if (ImGui::Checkbox(sus_label, &is_sus)) {
            active_env->sustain_step = is_sus ? selected_step_ : 0;
            modified = true;
        }
        CzTutorial::instance().tooltip("sustain");

        const char* end_label = tr("env.end");
        const float end_w = frame_h + inner_spacing_x + ImGui::CalcTextSize(end_label, nullptr, true).x;
        same_line_if_fits_ctrl(end_w);
        // End toggle
        bool is_end = (active_env->end_step == selected_step_);
        if (ImGui::Checkbox(end_label, &is_end)) {
            active_env->end_step = is_end ? selected_step_ : 8;
            modified = true;
        }
        CzTutorial::instance().tooltip("end_step");
    }

    return modified;
}

void CzEnvelopeGraph::render_casio_spec_plate(const CzPatch& patch, float scale) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImFont* font = ImGui::GetFont();
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const float s = (std::max)(scale, 0.75f);
    const float w = std::floor(590.0f * s);
    const float h = std::floor(280.0f * s);

    // Color palette matching authentic 1985 Casio CZ-3000 silkscreen plate
    const ImU32 col_bg         = IM_COL32(20, 23, 28, 255);
    const ImU32 col_hdr_bg     = IM_COL32(28, 34, 42, 255);
    const ImU32 col_sub_hdr    = IM_COL32(25, 31, 39, 255);
    const ImU32 col_outer_bdr  = IM_COL32(95, 122, 148, 220);
    const ImU32 col_grid       = IM_COL32(102, 134, 164, 210);
    const ImU32 col_grid_dim   = IM_COL32(62, 82, 104, 185);
    const ImU32 col_amber      = IM_COL32(238, 148, 48, 255);
    const ImU32 col_amber_dim  = IM_COL32(238, 148, 48, 34);
    const ImU32 col_cyan       = IM_COL32(106, 190, 235, 255);
    const ImU32 col_white      = IM_COL32(220, 230, 240, 255);
    const ImU32 col_badge_bg   = IM_COL32(148, 186, 218, 235);
    const ImU32 col_badge_txt  = IM_COL32(18, 21, 26, 255);

    auto txt_center = [&](float x0, float y0, float x1, float y1, float fsz, ImU32 col, const char* txt) {
        float sf = fsz * s;
        ImVec2 sz = font->CalcTextSizeA(sf, 2000.0f, 0.0f, txt);
        float tx = x0 + ((x1 - x0) - sz.x) * 0.5f;
        float ty = y0 + ((y1 - y0) - sz.y) * 0.5f;
        dl->AddText(font, sf, ImVec2(tx, ty), col, txt);
    };

    auto txt_left = [&](float x, float y0, float y1, float fsz, ImU32 col, const char* txt) {
        float sf = fsz * s;
        ImVec2 sz = font->CalcTextSizeA(sf, 2000.0f, 0.0f, txt);
        float ty = y0 + ((y1 - y0) - sz.y) * 0.5f;
        dl->AddText(font, sf, ImVec2(x, ty), col, txt);
    };

    auto draw_badge = [&](float cx, float cy, float fsz, const char* txt, ImU32 bg = 0, ImU32 fg = 0) {
        if (bg == 0) bg = col_badge_bg;
        if (fg == 0) fg = col_badge_txt;
        float sf = fsz * s;
        ImVec2 sz = font->CalcTextSizeA(sf, 2000.0f, 0.0f, txt);
        float pw = sz.x + 6.0f * s;
        float ph = sz.y + 1.0f * s;
        dl->AddRectFilled(ImVec2(cx - pw * 0.5f, cy - ph * 0.5f),
                          ImVec2(cx + pw * 0.5f, cy + ph * 0.5f), bg, 2.0f * s);
        dl->AddText(font, sf, ImVec2(cx - sz.x * 0.5f, cy - sz.y * 0.5f), fg, txt);
    };

    auto draw_dashed_h = [&](float x0, float x1, float y, ImU32 col, float dash, float gap, float thick) {
        float ds = dash * s, gs = gap * s;
        for (float x = x0; x < x1; x += (ds + gs)) {
            float xe = (std::min)(x + ds, x1);
            dl->AddLine(ImVec2(x, y), ImVec2(xe, y), col, thick);
        }
    };

    // Outer metallic faceplate frame
    dl->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h), col_bg, 4.0f * s);
    dl->AddRect(pos, ImVec2(pos.x + w, pos.y + h), col_outer_bdr, 4.0f * s, 0, 1.4f * s);

    const float pad = 6.0f * s;
    const float ix0 = pos.x + pad;
    const float ix1 = pos.x + w - pad;
    const float col_x = ix0 + std::floor((ix1 - ix0) * 0.34f);

    // =========================================================================
    // TOP BLOCK: VIBRATO WAVE (38s) + ENVELOPE SCHEMATICS (66s)
    // =========================================================================
    const float top_y0 = pos.y + 6.0f * s;
    const float y_vib1 = top_y0 + 38.0f * s;
    const float y_env1 = y_vib1 + 66.0f * s;

    dl->AddRect(ImVec2(ix0, top_y0), ImVec2(ix1, y_env1), col_grid, 0.0f, 0, 1.2f * s);
    dl->AddLine(ImVec2(col_x, top_y0), ImVec2(col_x, y_env1), col_grid, 1.1f * s);
    dl->AddLine(ImVec2(ix0, y_vib1), ImVec2(ix1, y_vib1), col_grid, 1.1f * s);

    txt_center(ix0, top_y0, col_x, y_vib1, 12.0f, col_amber, "VIBRATO WAVE");
    txt_center(ix0, y_vib1, col_x, y_env1, 12.0f, col_amber, "ENVELOPE");

    // 4 Vibrato Wave Oscilloscope Boxes
    {
        const float area_w = ix1 - col_x;
        const float box_w = 50.0f * s;
        const float box_h = 28.0f * s;
        const float gap = 7.0f * s;
        const float total_bw = box_w * 4.0f + gap * 3.0f;
        float bx = col_x + (area_w - total_bw) * 0.5f;
        float by = top_y0 + 5.0f * s;

        for (int i = 1; i <= 4; ++i) {
            bool active_w = (patch.vibrato.wave == i);
            ImVec2 b0(bx, by);
            ImVec2 b1(bx + box_w, by + box_h);
            dl->AddRectFilled(b0, b1, active_w ? col_amber_dim : IM_COL32(14, 17, 22, 255), 1.5f * s);
            dl->AddRect(b0, b1, active_w ? col_amber : col_grid, 1.5f * s, 0, (active_w ? 1.5f : 1.0f) * s);

            char num[4];
            snprintf(num, sizeof(num), "%d", i);
            txt_center(bx, by + 1.0f * s, bx + box_w, by + 11.0f * s, 10.0f,
                       active_w ? col_amber : col_white, num);

            const float wx0 = bx + 8.0f * s;
            const float wx1 = bx + box_w - 8.0f * s;
            const float wy0 = by + 12.0f * s;
            const float wy1 = by + box_h - 4.0f * s;
            const float wym = (wy0 + wy1) * 0.5f;
            const float ww = wx1 - wx0;

            if (i == 1) { // Triangle wave
                ImVec2 pts[4] = {
                    {wx0, wym},
                    {wx0 + ww * 0.25f, wy0},
                    {wx0 + ww * 0.75f, wy1},
                    {wx1, wym}
                };
                dl->AddPolyline(pts, 4, col_amber, ImDrawFlags_None, 1.6f * s);
            } else if (i == 2) { // Saw Up
                ImVec2 pts[4] = {
                    {wx0, wy1},
                    {wx1 - 4.0f * s, wy0},
                    {wx1 - 4.0f * s, wy1},
                    {wx1, wy1}
                };
                dl->AddPolyline(pts, 4, col_amber, ImDrawFlags_None, 1.6f * s);
            } else if (i == 3) { // Saw Down
                ImVec2 pts[3] = {
                    {wx0 + 2.0f * s, wy1},
                    {wx0 + 2.0f * s, wy0},
                    {wx1, wy1}
                };
                dl->AddPolyline(pts, 3, col_amber, ImDrawFlags_None, 1.6f * s);
            } else { // Square wave (2 cycles)
                const float s1 = wx0 + ww * 0.28f;
                const float s2 = wx0 + ww * 0.55f;
                const float s3 = wx0 + ww * 0.82f;
                ImVec2 pts[9] = {
                    {wx0, wy1}, {wx0, wy0}, {s1, wy0}, {s1, wy1},
                    {s2, wy1}, {s2, wy0}, {s3, wy0}, {s3, wy1}, {wx1, wy1}
                };
                dl->AddPolyline(pts, 9, col_amber, ImDrawFlags_None, 1.5f * s);
            }
            bx += box_w + gap;
        }
    }

    // ENVELOPE Row: Two Vector Schematic Diagrams side by side
    {
        const float rx0 = col_x + 8.0f * s;
        const float rx1 = ix1 - 8.0f * s;
        const float ry0 = y_vib1 + 4.0f * s;
        const float ry1 = y_env1 - 4.0f * s;
        const float rw = rx1 - rx0;

        // Left Schematic: PITCH | WAVE | AMP matrix (~35% width)
        const float mx0 = rx0;
        const float mx1 = rx0 + std::floor(rw * 0.35f);
        const float m_foot_y = ry1 - 11.0f * s;
        const float m_grid_h = m_foot_y - ry0;
        const float c1 = mx0 + (mx1 - mx0) * 0.33f;
        const float c2 = mx0 + (mx1 - mx0) * 0.66f;

        dl->AddRect(ImVec2(mx0, ry0), ImVec2(mx1, ry1), col_grid, 0.0f, 0, 1.0f * s);
        dl->AddRectFilled(ImVec2(mx0 + 1.0f * s, m_foot_y), ImVec2(mx1 - 1.0f * s, ry1 - 1.0f * s), col_badge_bg);
        dl->AddLine(ImVec2(c1, ry0), ImVec2(c1, ry1), col_grid, 1.0f * s);
        dl->AddLine(ImVec2(c2, ry0), ImVec2(c2, ry1), col_grid, 1.0f * s);
        dl->AddLine(ImVec2(mx0, m_foot_y), ImVec2(mx1, m_foot_y), col_grid, 1.0f * s);

        for (int g = 1; g <= 3; ++g) {
            float gy = ry0 + (m_grid_h * (g / 4.0f));
            dl->AddLine(ImVec2(mx0, gy), ImVec2(mx1, gy), col_grid_dim, 1.0f * s);
        }

        txt_center(mx0, m_foot_y, c1, ry1, 9.2f, col_badge_txt, "PITCH");
        txt_center(c1, m_foot_y, c2, ry1, 9.2f, col_badge_txt, "WAVE");
        txt_center(c2, m_foot_y, mx1, ry1, 9.2f, col_badge_txt, "AMP.");

        txt_center(mx0, ry0, c1, ry0 + m_grid_h * 0.28f, 9.2f, col_white, "6OCT");
        txt_center(mx0, m_foot_y - m_grid_h * 0.28f, c1, m_foot_y, 9.2f, col_white, "0");

        txt_center(c2, ry0, mx1, ry0 + m_grid_h * 0.28f, 9.2f, col_white, "MAX");
        txt_center(c2, m_foot_y - m_grid_h * 0.28f, mx1, m_foot_y, 9.2f, col_white, "MIN");

        // WAVE progression micro-icons
        {
            const float wcx0 = c1 + 6.0f * s;
            const float wcx1 = c2 - 6.0f * s;
            const float wcm = (wcx0 + wcx1) * 0.5f;
            float ty = ry0 + m_grid_h * 0.14f;
            ImVec2 p_top[5] = {{wcx0, ty + 3 * s}, {wcx0 + (wcm - wcx0) * 0.5f, ty - 3 * s}, {wcm, ty + 3 * s}, {wcx1 - 2 * s, ty - 3 * s}, {wcx1, ty + 3 * s}};
            dl->AddPolyline(p_top, 5, col_amber, ImDrawFlags_None, 1.3f * s);

            float my = ry0 + m_grid_h * 0.50f;
            ImVec2 p_mid[5] = {{wcx0, my + 2 * s}, {wcx0 + 5 * s, my - 3 * s}, {wcm, my + 1 * s}, {wcx1 - 5 * s, my - 3 * s}, {wcx1, my + 2 * s}};
            dl->AddPolyline(p_mid, 5, col_amber, ImDrawFlags_None, 1.3f * s);

            float by2 = ry0 + m_grid_h * 0.86f;
            ImVec2 p_bot[5] = {{wcx0, by2}, {wcx0 + (wcm - wcx0) * 0.5f, by2 - 2.5f * s}, {wcm, by2}, {wcm + (wcx1 - wcm) * 0.5f, by2 + 2.5f * s}, {wcx1, by2}};
            dl->AddPolyline(p_bot, 5, col_amber, ImDrawFlags_None, 1.3f * s);
        }

        // Right Schematic: 8-Step Envelope Curve Diagram (~62% width)
        const float ex0 = mx1 + 8.0f * s;
        const float ex1 = rx1;
        const float e_foot_y = m_foot_y;
        const float ew = ex1 - ex0;
        const float eh = e_foot_y - ry0;

        dl->AddRect(ImVec2(ex0, ry0), ImVec2(ex1, ry1), col_grid, 0.0f, 0, 1.0f * s);
        dl->AddRectFilled(ImVec2(ex0 + 1.0f * s, e_foot_y), ImVec2(ex1 - 1.0f * s, ry1 - 1.0f * s), col_badge_bg);
        txt_center(ex0, e_foot_y, ex1, ry1, 9.2f, col_badge_txt, "ENVELOPE");

        for (int g = 1; g <= 3; ++g) {
            float gy = ry0 + eh * (g / 4.0f);
            dl->AddLine(ImVec2(ex0, gy), ImVec2(ex1, gy), col_grid_dim, 1.0f * s);
        }

        // Extra headroom at top (ry0 + 14*s) and bottom (e_foot_y - 10*s) so badges never touch curves
        const float sx0 = ex0 + 16.0f * s;
        const float sx_span = ew - 34.0f * s;
        const float y_base = e_foot_y - 9.0f * s;
        const float y_hi = ry0 + 13.0f * s;
        auto px = [&](float t) { return sx0 + sx_span * t; };
        auto py = [&](float v) { return y_base - (y_base - y_hi) * v; };

        ImVec2 curve[10] = {
            {px(0.00f), py(0.00f)}, // KEY ON
            {px(0.08f), py(0.55f)}, // 1
            {px(0.16f), py(0.95f)}, // 2
            {px(0.28f), py(0.45f)}, // 3
            {px(0.39f), py(0.18f)}, // 4
            {px(0.51f), py(0.72f)}, // 5 (SUS)
            {px(0.73f), py(0.72f)}, // Sustain hold -> KEY OFF
            {px(0.81f), py(0.38f)}, // 6
            {px(0.90f), py(0.28f)}, // 7
            {px(0.99f), py(0.00f)}  // 8 (END)
        };

        dl->AddLine(ImVec2(curve[5].x, ry0), ImVec2(curve[5].x, e_foot_y), col_grid, 1.0f * s);
        dl->AddLine(ImVec2(curve[6].x, ry0), ImVec2(curve[6].x, e_foot_y), col_grid, 1.0f * s);

        dl->AddPolyline(curve, 6, col_amber, ImDrawFlags_None, 1.5f * s);
        draw_dashed_h(curve[5].x, curve[6].x, curve[5].y, col_amber, 4.0f, 3.0f, 1.5f * s);
        dl->AddPolyline(&curve[6], 4, col_amber, ImDrawFlags_None, 1.5f * s);

        const char* s_nums[8] = {"1", "2", "3", "4", "5", "6", "7", "8"};
        ImVec2 n_pts[8] = {
            {curve[1].x - 7 * s, curve[1].y - 7 * s},
            {curve[2].x + 3 * s, curve[2].y - 3 * s},
            {curve[3].x - 7 * s, curve[3].y + 1 * s},
            {curve[4].x + 3 * s, curve[4].y - 8 * s},
            {curve[5].x - 8 * s, curve[5].y - 9 * s},
            {curve[7].x - 6 * s, curve[7].y - 9 * s},
            {curve[8].x + 3 * s, curve[8].y - 7 * s},
            {curve[9].x - 7 * s, curve[9].y - 9 * s}
        };
        for (int k = 0; k < 8; ++k) {
            dl->AddText(font, 8.8f * s, n_pts[k], col_white, s_nums[k]);
        }

        draw_badge((curve[5].x + curve[6].x) * 0.5f, ry0 + 6.0f * s, 8.0f, "SUSTAIN");
        draw_badge((curve[6].x + ex1) * 0.5f, ry0 + 6.0f * s, 8.0f, "AFTER ENV.");
        draw_badge(curve[0].x + 12.0f * s, e_foot_y - 5.5f * s, 8.0f, "KEY ON");
        draw_badge(curve[5].x, e_foot_y - 5.5f * s, 8.0f, "SUS");
        draw_badge(curve[6].x, e_foot_y - 5.5f * s, 8.0f, "KEY OFF");
        draw_badge(curve[9].x - 6.0f * s, e_foot_y - 5.5f * s, 8.0f, "END");
    }

    // =========================================================================
    // BOTTOM BLOCK: PARAMETER | VARIABLE RANGE TABLE
    // =========================================================================
    const float tbl_y0 = y_env1 + 5.0f * s;
    const float r_hdr  = tbl_y0 + 18.0f * s;
    const float r_vib  = r_hdr  + 17.0f * s;
    const float r_oct  = r_vib  + 17.0f * s;
    const float r_wave = r_oct  + 26.0f * s;
    const float r_env  = r_wave + 45.0f * s;
    const float r_kf   = r_env  + 17.0f * s;
    const float r_det  = r_kf   + 19.0f * s; // total ~ 274*s inside h=280*s

    dl->AddRectFilled(ImVec2(ix0, tbl_y0), ImVec2(ix1, r_hdr), col_hdr_bg);
    dl->AddRect(ImVec2(ix0, tbl_y0), ImVec2(ix1, r_det), col_grid, 0.0f, 0, 1.2f * s);
    dl->AddLine(ImVec2(col_x, tbl_y0), ImVec2(col_x, r_det), col_grid, 1.1f * s);

    float row_ys[] = {r_hdr, r_vib, r_oct, r_wave, r_env, r_kf};
    for (float ry : row_ys) {
        dl->AddLine(ImVec2(ix0, ry), ImVec2(ix1, ry), col_grid, 1.0f * s);
    }

    txt_center(ix0, tbl_y0, col_x, r_hdr, 11.5f, col_amber, "PARAMETER");
    txt_center(col_x, tbl_y0, ix1, r_hdr, 11.5f, col_amber, "VARIABLE RANGE");

    const float vpad = col_x + 8.0f * s;
    txt_center(ix0, r_hdr, col_x, r_vib, 10.8f, col_cyan, "VIBRATO");
    txt_left(vpad, r_hdr, r_vib, 10.8f, col_white, "WAVE=1~4,  DELAY=00~99,  RATE=00~99,  DEPTH=00~99");

    txt_center(ix0, r_vib, col_x, r_oct, 10.8f, col_cyan, "OCTAVE / LINE");
    txt_left(vpad, r_vib, r_oct, 10.8f, col_white, "RANGE=-1~0~+1   |   LINE: 1, 2, 1+1', 1+2'   |   RING / NOISE");

    txt_center(ix0, r_oct, col_x, r_oct + 13.0f * s, 10.8f, col_cyan, "WAVE FORM");
    txt_center(ix0, r_oct + 13.0f * s, col_x, r_wave, 9.8f, col_cyan, "(DCO1, DCO2)");
    txt_left(vpad, r_oct, r_oct + 13.0f * s, 10.5f, col_white, "FIRST  = 1~8  (1:Saw 2:Sqr 3:Pls 4:DblSin 5:SawPls 6-8:Res I-III)");
    txt_left(vpad, r_oct + 13.0f * s, r_wave, 10.5f, col_white, "SECOND = 0~8  (0:Off, 1~8:Alternate Cycle Wave)");

    // Row 4: PITCH, WAVE, AMP ENVELOPE + 4-col Sub-Table
    txt_center(ix0, r_wave, col_x, r_wave + 15.0f * s, 10.2f, col_cyan, "PITCH, WAVE, AMP");
    txt_center(ix0, r_wave + 15.0f * s, col_x, r_wave + 30.0f * s, 10.2f, col_cyan, "ENVELOPE");
    txt_center(ix0, r_wave + 30.0f * s, col_x, r_env, 9.2f, col_cyan, "(DCO1,2)(DCW1,2)(DCA1,2)");

    {
        const float st_w = ix1 - col_x;
        const float sc1 = col_x + st_w * 0.24f;
        const float sc2 = col_x + st_w * 0.49f;
        const float sc3 = col_x + st_w * 0.75f;
        const float sy1 = r_wave + 15.0f * s;
        const float sy2 = r_wave + 30.0f * s;

        dl->AddRectFilled(ImVec2(col_x + 1.0f * s, r_wave + 1.0f * s), ImVec2(ix1 - 1.0f * s, sy1), col_sub_hdr);
        dl->AddLine(ImVec2(col_x, sy1), ImVec2(ix1, sy1), col_grid, 1.0f * s);
        dl->AddLine(ImVec2(col_x, sy2), ImVec2(ix1, sy2), col_grid, 1.0f * s);
        dl->AddLine(ImVec2(sc1, r_wave), ImVec2(sc1, r_env), col_grid, 1.0f * s);
        dl->AddLine(ImVec2(sc2, r_wave), ImVec2(sc2, r_env), col_grid, 1.0f * s);
        dl->AddLine(ImVec2(sc3, r_wave), ImVec2(sc3, r_env), col_grid, 1.0f * s);

        txt_center(sc1, r_wave, sc2, sy1, 10.2f, col_white, "STEP 1");
        txt_center(sc2, r_wave, sc3, sy1, 10.2f, col_white, "STEP 2~7");
        txt_center(sc3, r_wave, ix1, sy1, 10.2f, col_white, "STEP 8");

        txt_center(col_x, sy1, sc1, sy2, 10.2f, col_white, "RATE");
        txt_center(sc1, sy1, sc2, sy2, 10.2f, col_white, "00~99");
        txt_center(sc2, sy1, sc3, sy2, 10.2f, col_white, "00~99");
        txt_center(sc3, sy1, ix1, sy2, 10.2f, col_white, "00~99");

        txt_center(col_x, sy2, sc1, r_env, 10.2f, col_white, "LEVEL");
        txt_center(sc1, sy2, sc2, r_env, 10.2f, col_white, "00~99");
        txt_center(sc2, sy2, sc3, r_env, 10.2f, col_white, "00~99");
        txt_center(sc3, sy2, ix1, r_env, 10.2f, col_white, "00");
    }

    txt_center(ix0, r_env, col_x, r_kf, 10.2f, col_cyan, "DCW, DCA KEY FOLLOW");
    txt_left(vpad, r_env, r_kf, 10.8f, col_white, "RANGE=0~9");

    txt_center(ix0, r_kf, col_x, r_det, 10.8f, col_cyan, "DETUNE");
    txt_left(vpad, r_kf, r_det, 10.8f, col_white, "OCTAVE=0~3,  NOTE=00~11,  FINE=00~60  (POLARITY +/-)");

    ImGui::Dummy(ImVec2(w, h));
}

} // namespace cz

