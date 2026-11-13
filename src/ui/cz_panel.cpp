#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "cz_panel.h"
#include "../core/cz_sysex.h"
#include "cz_tutorial.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

namespace cz {

namespace {

static const char *FindTextEnd(const char *text) {
  const char *text_end = text;
  while (*text_end != '\0' && (text_end[0] != '#' || text_end[1] != '#'))
    text_end++;
  return text_end;
}

// Helper to draw a tactile Casio CZ button with an LED indicator
bool CasioButton(const char *label, bool active, bool led = false,
                 ImVec2 size = ImVec2(52, 38),
                 ImU32 led_color = IM_COL32(255, 40, 40, 255)) {
  ImDrawList *draw = ImGui::GetWindowDrawList();
  ImVec2 pos = ImGui::GetCursorScreenPos();

  bool pressed = ImGui::InvisibleButton(label, size);
  bool hovered = ImGui::IsItemHovered();
  bool held = ImGui::IsItemActive();

  // Button body color
  ImU32 btn_col = active ? IM_COL32(55, 60, 68, 255)
                         : (hovered ? IM_COL32(45, 50, 56, 255)
                                    : IM_COL32(36, 40, 45, 255));
  if (held)
    btn_col = IM_COL32(25, 28, 32, 255);

  // Bevel edges
  ImVec2 p_min = pos;
  ImVec2 p_max = ImVec2(pos.x + size.x, pos.y + size.y);
  draw->AddRectFilled(p_min, p_max, btn_col, 3.0f);
  draw->AddRect(p_min, p_max,
                held ? IM_COL32(20, 20, 20, 255) : IM_COL32(70, 78, 88, 255),
                3.0f, 0, 1.0f);

  // Top highlight line
  draw->AddLine(ImVec2(p_min.x + 2, p_min.y + 1),
                ImVec2(p_max.x - 2, p_min.y + 1),
                IM_COL32(255, 255, 255, held ? 10 : 40));

  // LED Indicator (round dot near top of button)
  if (led || active) {
    ImVec2 led_pos = ImVec2(pos.x + (size.x * 0.5f), pos.y + 6.0f);
    ImU32 col = active ? led_color : IM_COL32(60, 20, 20, 255);
    draw->AddCircleFilled(led_pos, 3.0f, col);
    if (active) {
      // LED Glow halo
      draw->AddCircleFilled(led_pos, 5.0f,
                            (led_color & 0x00FFFFFF) | 0x50000000);
    }
  }

  // Button text label (render only text before "##")
  const char *text_end = FindTextEnd(label);
  ImVec2 text_size = ImGui::CalcTextSize(label, text_end);
  float text_y = pos.y + (size.y * 0.5f) - (text_size.y * 0.5f) +
                 (led || active ? 3.0f : 0.0f);
  draw->AddText(ImVec2(pos.x + (size.x - text_size.x) * 0.5f, text_y),
                IM_COL32(230, 235, 240, 255), label, text_end);

                return pressed;
}

} // anonymous namespace

CzPanel::CzPanel(CzBank &bank, CzMidiManager &midi)
    : bank_(bank), midi_(midi), librarian_(bank, midi) {
  select_patch(0);
}

void CzPanel::select_patch(size_t index) {
  active_patch_index_ =
      std::clamp(index, static_cast<size_t>(0), CzBank::BANK_SIZE - 1);
  edit_patch_ = bank_.get_patch_by_index(active_patch_index_);
  compare_patch_ = edit_patch_;
  compare_active_ = false;
  current_page_ = PanelPage::Play;
  update_lcd();
}

void CzPanel::update_lcd() {
  char l1[32];
  char l2[32];

  const CzPatch &p = compare_active_ ? compare_patch_ : edit_patch_;
  const CzLine &line = (current_line_ == 1) ? p.line1 : p.line2;

  switch (current_page_) {
  case PanelPage::Play: {
    snprintf(l1, sizeof(l1), "%c%d: %-12.12s", p.memory.bank, p.memory.slot,
             p.name.c_str());
    const char *ls_str = "1";
    if (p.line_select == LineSelect::Line2)
      ls_str = "2";
    else if (p.line_select == LineSelect::Line1_Plus_1Prime)
      ls_str = "1+1'";
    else if (p.line_select == LineSelect::Line1_Plus_2Prime)
      ls_str = "1+2'";
    snprintf(l2, sizeof(l2), "OCT:%+d L:%-4s INT", p.octave, ls_str);
    lcd_.set_cursor(0, -1, false);
    break;
  }
  case PanelPage::Dco: {
    snprintf(l1, sizeof(l1), "DCO%d W1:%d W2:%d%s", current_line_,
             line.wave_1 + 1, line.wave_2 + 1, line.use_wave_2 ? "+" : " ");
    const auto &step = line.dco.steps[active_step_ - 1];
    snprintf(l2, sizeof(l2), "ST%d R:%02d L:%02d%s", active_step_, step.rate,
             step.level,
             (line.dco.sustain_step == active_step_)
                 ? "S"
                 : (line.dco.end_step == active_step_ ? "E" : " "));
    lcd_.set_cursor(1, (cursor_param_ == 0) ? 6 : 11, true);
    break;
  }
  case PanelPage::Dcw: {
    snprintf(l1, sizeof(l1), "DCW%d KEY FOL:%d ", current_line_,
             line.dcw.key_follow);
    const auto &step = line.dcw.steps[active_step_ - 1];
    snprintf(l2, sizeof(l2), "ST%d R:%02d L:%02d%s", active_step_, step.rate,
             step.level,
             (line.dcw.sustain_step == active_step_)
                 ? "S"
                 : (line.dcw.end_step == active_step_ ? "E" : " "));
    lcd_.set_cursor(1, (cursor_param_ == 0) ? 6 : 11, true);
    break;
  }
  case PanelPage::Dca: {
    snprintf(l1, sizeof(l1), "DCA%d KEY FOL:%d ", current_line_,
             line.dca.key_follow);
    const auto &step = line.dca.steps[active_step_ - 1];
    snprintf(l2, sizeof(l2), "ST%d R:%02d L:%02d%s", active_step_, step.rate,
             step.level,
             (line.dca.sustain_step == active_step_)
                 ? "S"
                 : (line.dca.end_step == active_step_ ? "E" : " "));
    lcd_.set_cursor(1, (cursor_param_ == 0) ? 6 : 11, true);
    break;
  }
  case PanelPage::Vibrato: {
    snprintf(l1, sizeof(l1), "VIB  WV:%d DLY:%02d", p.vibrato.wave,
             p.vibrato.delay);
    snprintf(l2, sizeof(l2), "RATE:%02d DEPTH:%02d", p.vibrato.rate,
             p.vibrato.depth);
    lcd_.set_cursor((cursor_param_ < 2) ? 0 : 1,
                    (cursor_param_ % 2 == 0) ? 7 : 14, true);
    break;
  }
  case PanelPage::Detune: {
    snprintf(l1, sizeof(l1), "DETUNE  %-3s  O:%d",
             p.detune.negative ? "NEG" : "POS", p.detune.octave);
    snprintf(l2, sizeof(l2), "NOTE:%02d  FINE:%02d", p.detune.note,
             p.detune.fine);
    lcd_.set_cursor((cursor_param_ < 2) ? 0 : 1,
                    (cursor_param_ % 2 == 0) ? 7 : 14, true);
    break;
  }
  case PanelPage::LineSelect: {
    const char *ls_str = "1";
    if (p.line_select == LineSelect::Line2)
      ls_str = "2";
    else if (p.line_select == LineSelect::Line1_Plus_1Prime)
      ls_str = "1+1'";
    else if (p.line_select == LineSelect::Line1_Plus_2Prime)
      ls_str = "1+2'";
    snprintf(l1, sizeof(l1), "LINE SELECT %-4s", ls_str);
    snprintf(l2, sizeof(l2), "RING:%-3s NOISE:%-3s",
             (p.line1.modulation == 1) ? "ON" : "OFF",
             (p.line1.modulation == 2) ? "ON" : "OFF");
    lcd_.set_cursor(0, 12, true);
    break;
  }
  case PanelPage::Status:
    break;
  }

  lcd_.set_lines(l1, l2);
}

void CzPanel::increment_current_parameter(int delta) {
  CzLine &line = (current_line_ == 1) ? edit_patch_.line1 : edit_patch_.line2;

  switch (current_page_) {
  case PanelPage::Play: {
    int new_idx = static_cast<int>(active_patch_index_) + delta;
    if (new_idx >= 0 && new_idx < static_cast<int>(CzBank::BANK_SIZE)) {
      select_patch(static_cast<size_t>(new_idx));
    }
    break;
  }
  case PanelPage::Dco: {
    auto &s = line.dco.steps[active_step_ - 1];
    if (cursor_param_ == 0)
      s.rate = static_cast<uint8_t>(std::clamp(s.rate + delta, 0, 99));
    else
      s.level = static_cast<uint8_t>(std::clamp(s.level + delta, 0, 99));
    break;
  }
  case PanelPage::Dcw: {
    auto &s = line.dcw.steps[active_step_ - 1];
    if (cursor_param_ == 0)
      s.rate = static_cast<uint8_t>(std::clamp(s.rate + delta, 0, 99));
    else
      s.level = static_cast<uint8_t>(std::clamp(s.level + delta, 0, 99));
    break;
  }
  case PanelPage::Dca: {
    auto &s = line.dca.steps[active_step_ - 1];
    if (cursor_param_ == 0)
      s.rate = static_cast<uint8_t>(std::clamp(s.rate + delta, 0, 99));
    else
      s.level = static_cast<uint8_t>(std::clamp(s.level + delta, 0, 99));
    break;
  }
  case PanelPage::Vibrato: {
    if (cursor_param_ == 0)
      edit_patch_.vibrato.wave =
          std::clamp(edit_patch_.vibrato.wave + delta, 1, 4);
    else if (cursor_param_ == 1)
      edit_patch_.vibrato.delay =
          std::clamp(edit_patch_.vibrato.delay + delta, 0, 99);
    else if (cursor_param_ == 2)
      edit_patch_.vibrato.rate =
          std::clamp(edit_patch_.vibrato.rate + delta, 0, 99);
    else if (cursor_param_ == 3)
      edit_patch_.vibrato.depth =
          std::clamp(edit_patch_.vibrato.depth + delta, 0, 99);
    break;
  }
  case PanelPage::Detune: {
    if (cursor_param_ == 0)
      edit_patch_.detune.negative = !edit_patch_.detune.negative;
    else if (cursor_param_ == 1)
      edit_patch_.detune.octave =
          std::clamp(edit_patch_.detune.octave + delta, 0, 3);
    else if (cursor_param_ == 2)
      edit_patch_.detune.note =
          std::clamp(edit_patch_.detune.note + delta, 0, 11);
    else if (cursor_param_ == 3)
      edit_patch_.detune.fine =
          std::clamp(edit_patch_.detune.fine + delta, 0, 60);
    break;
  }
  case PanelPage::LineSelect: {
    int ls = static_cast<int>(edit_patch_.line_select) + delta;
    if (ls < 0)
      ls = 3;
    if (ls > 3)
      ls = 0;
    edit_patch_.line_select = static_cast<LineSelect>(ls);
    break;
  }
  case PanelPage::Status:
    break;
  }

  bank_.patches[active_patch_index_] = edit_patch_;
  update_lcd();
}

void CzPanel::render() {
  render_header_bar();
  render_hardware_chassis();

  if (show_envelope_graph_) {
    ImGui::Spacing();
    ImGui::Separator();
    envelope_graph_.render(edit_patch_, current_line_);
  }

  render_bottom_toolbar();

  // Tutorial modal guide if requested
  if (show_tutorial_window_) {
    CzTutorial::instance().render_guide_window(&show_tutorial_window_);
  }
}

void CzPanel::render_header_bar() {
  ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(22, 25, 28, 255));
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Archivo")) {
      if (ImGui::MenuItem("Cargar Archivo SysEx (.syx)...")) {
        // To be opened via file dialog
      }
      if (ImGui::MenuItem("Guardar Banco SysEx (.syx)...")) {
        // To be saved via file dialog
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Restablecer Banco de Fábrica")) {
        bank_.initialize_factory_presets();
        select_patch(0);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Vistas")) {
      ImGui::MenuItem("Gráfico de Envolventes", nullptr, &show_envelope_graph_);
      ImGui::MenuItem("Gestor de Biblioteca (Librarian)", nullptr,
                      &show_librarian_);
      ImGui::MenuItem("Guía Didáctica de Distorsión de Fase", nullptr,
                      &show_tutorial_window_);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("MIDI")) {
      auto in_devs = midi_.get_input_devices();
      auto out_devs = midi_.get_output_devices();

      ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f),
                         "Puerto de Entrada (In):");
      for (const auto &dev : in_devs) {
        bool sel = (midi_.current_input_id() == dev.id);
        if (ImGui::MenuItem(dev.name.c_str(), nullptr, sel)) {
          midi_.open_input(dev.id);
        }
      }
      if (in_devs.empty())
        ImGui::TextDisabled("  (Sin dispositivos MIDI In)");

      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f),
                         "Puerto de Salida (Out):");
      for (const auto &dev : out_devs) {
        bool sel = (midi_.current_output_id() == dev.id);
        if (ImGui::MenuItem(dev.name.c_str(), nullptr, sel)) {
          midi_.open_output(dev.id);
        }
      }
      if (out_devs.empty())
        ImGui::TextDisabled("  (Sin dispositivos MIDI Out)");

      ImGui::EndMenu();
    }

    // Right side prominent Tutorial button
    float tut_w = 210.0f;
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - tut_w - 10.0f);

    bool tut_on = CzTutorial::instance().is_enabled();
    if (tut_on) {
      ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 140, 150, 255));
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    } else {
      ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(60, 60, 60, 255));
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
    }

    if (ImGui::Button(tut_on ? "[?] MODO TUTORIAL: ON"
                             : "[?] MODO TUTORIAL: OFF",
                      ImVec2(180, 20))) {
      CzTutorial::instance().toggle();
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    if (ImGui::SmallButton("Guía")) {
      show_tutorial_window_ = true;
    }

    ImGui::EndMainMenuBar();
  }
  ImGui::PopStyleColor();
}

void CzPanel::render_hardware_chassis() {
  ImDrawList *draw = ImGui::GetWindowDrawList();
  ImVec2 panel_pos = ImGui::GetCursorScreenPos();
  float panel_w = ImGui::GetContentRegionAvail().x;
  float panel_h = 345.0f;

  // Outer dark chassis body
  draw->AddRectFilled(panel_pos,
                      ImVec2(panel_pos.x + panel_w, panel_pos.y + panel_h),
                      IM_COL32(28, 31, 35, 255), 6.0f);
  draw->AddRect(panel_pos, ImVec2(panel_pos.x + panel_w, panel_pos.y + panel_h),
                IM_COL32(50, 56, 64, 255), 6.0f, 0, 2.0f);

  // Decorative Casio colored racing stripes (Cyan, Dark Blue, Red)
  float stripe_y = panel_pos.y + 36.0f;
  draw->AddLine(ImVec2(panel_pos.x + 10, stripe_y),
                ImVec2(panel_pos.x + panel_w - 10, stripe_y),
                IM_COL32(0, 175, 195, 255), 2.0f);
  draw->AddLine(ImVec2(panel_pos.x + 10, stripe_y + 3),
                ImVec2(panel_pos.x + panel_w - 10, stripe_y + 3),
                IM_COL32(30, 90, 180, 255), 1.5f);
  draw->AddLine(ImVec2(panel_pos.x + 10, stripe_y + 6),
                ImVec2(panel_pos.x + panel_w - 10, stripe_y + 6),
                IM_COL32(220, 45, 45, 255), 1.5f);

  // Header branding
  ImGui::SetCursorScreenPos(ImVec2(panel_pos.x + 16, panel_pos.y + 10));
  ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.95f, 1.0f), "CASIO");
  ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.0f, 0.85f, 0.95f, 1.0f), "CZ-3000");
  ImGui::SameLine();
  ImGui::TextColored(ImVec4(0.6f, 0.65f, 0.7f, 1.0f),
                     "PHASE DISTORTION SYNTHESIZER / SYSEX LIBRARIAN");

  // Internal grid layout (Left: Hardware Controls)
  ImGui::SetCursorScreenPos(ImVec2(panel_pos.x + 15, panel_pos.y + 50));

  // Section 1: Programmer & Master Controls (Left)
  ImGui::BeginGroup();
  render_programmer_section();
  ImGui::EndGroup();

  ImGui::SameLine(0, 15);

  // Section 2: 16x2 HD44780 LCD Screen (Center)
  ImGui::BeginGroup();
  lcd_.render(1.0f);
  ImGui::EndGroup();

  ImGui::SameLine(0, 15);

  // Section 3: Data Entry Slider & Buttons (Center-Right)
  ImGui::BeginGroup();
  render_data_entry_section();
  ImGui::EndGroup();

  // Section 4: Parameter & Bank Selector Buttons (Lower deck)
  ImGui::SetCursorScreenPos(ImVec2(panel_pos.x + 15, panel_pos.y + 170));
  render_parameter_buttons();

  ImGui::SetCursorScreenPos(ImVec2(panel_pos.x + 15, panel_pos.y + 255));
  render_bank_number_buttons();

  // Right Side: Integrated Bank Librarian (to the right of buttons & display)
  if (show_librarian_ && panel_w > 780.0f) {
    float div_x = panel_pos.x + 590.0f;
    draw->AddLine(ImVec2(div_x, panel_pos.y + 48.0f),
                  ImVec2(div_x, panel_pos.y + panel_h - 10.0f),
                  IM_COL32(60, 68, 78, 255), 1.5f);

    float right_w = panel_w - 610.0f;
    ImGui::SetCursorScreenPos(ImVec2(div_x + 10.0f, panel_pos.y + 48.0f));
    ImGui::BeginChild("RightLibrarian", ImVec2(right_w, panel_h - 58.0f), false);
    if (librarian_.render(active_patch_index_)) {
      select_patch(active_patch_index_);
    }
    ImGui::EndChild();
  }

  // Set cursor below the chassis
  ImGui::SetCursorScreenPos(ImVec2(panel_pos.x, panel_pos.y + panel_h + 15));
  ImGui::NewLine();
  render_audition_keyboard();
}

void CzPanel::render_programmer_section() {
  ImGui::PushID("Programmer");
  ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "PROGRAMMER");

  if (CasioButton("COMPARE##prog", compare_active_, true, ImVec2(56, 34),
                  IM_COL32(255, 180, 0, 255))) {
    compare_active_ = !compare_active_;
    update_lcd();
  }
  CzTutorial::instance().tooltip("sysex_io");
  ImGui::SameLine();

    if (CasioButton("RECALL##prog", false, false, ImVec2(56, 34))) {
      edit_patch_ = compare_patch_;
      compare_active_ = false;
      update_lcd();
    }
    CzTutorial::instance().tooltip("sysex_io");

      if (CasioButton("INIT##prog", false, false, ImVec2(56, 34))) {
        auto [b, s] = CzBank::get_bank_slot(active_patch_index_);
        edit_patch_ = CzPatch::create_default_patch(b, s, "INIT SOUND");
        compare_patch_ = edit_patch_;
        compare_active_ = false;
        update_lcd();
      }
      CzTutorial::instance().tooltip("sysex_io");
      ImGui::SameLine();

  if (CasioButton("WRITE##prog", false, true, ImVec2(56, 34),
                  IM_COL32(255, 40, 40, 255))) {
        bank_.patches[active_patch_index_] = edit_patch_;
        compare_patch_ = edit_patch_;
        // Send SysEx Write
        auto syx = CzSysEx::patch_to_sysex(edit_patch_, true);
        midi_.send_sysex(syx);
        librarian_.set_status_message(
            "Patch guardado en memoria y enviado al CZ.");
        update_lcd();
  }
  CzTutorial::instance().tooltip("sysex_io");
  ImGui::PopID();
    }

    void CzPanel::render_data_entry_section() {
      ImGui::PushID("DataEntry");
      ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "DATA ENTRY");

        if (CasioButton("UP##data", false, false, ImVec2(50, 32))) {
          increment_current_parameter(+1);
        }
        ImGui::SameLine();
          if (CasioButton("DOWN##data", false, false, ImVec2(50, 32))) {
            increment_current_parameter(-1);
          }

            if (CasioButton("< CUR##data", false, false, ImVec2(50, 32))) {
              cursor_param_ = (std::max)(0, cursor_param_ - 1);
              update_lcd();
            }
            ImGui::SameLine();
              if (CasioButton("CUR >##data", false, false, ImVec2(50, 32))) {
                cursor_param_ = (std::min)(3, cursor_param_ + 1);
                update_lcd();
              }
              ImGui::PopID();
            }

            void CzPanel::render_parameter_buttons() {
              ImGui::PushID("ParamDeck");
              ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.85f, 1.0f),
                                 "PARAMETER SELECT:");

              // Mode / Page buttons
  if (CasioButton("PLAY##param", current_page_ == PanelPage::Play, true,
                  ImVec2(46, 32), IM_COL32(0, 255, 180, 255))) {
                current_page_ = PanelPage::Play;
                update_lcd();
  }
  ImGui::SameLine(0, 5);

  if (CasioButton("LINE##param", current_page_ == PanelPage::LineSelect, true,
                  ImVec2(46, 32))) {
                current_page_ = PanelPage::LineSelect;
                update_lcd();
  }
  CzTutorial::instance().tooltip("line_select");
  ImGui::SameLine(0, 5);

  if (CasioButton("VIB##param", current_page_ == PanelPage::Vibrato, true,
                  ImVec2(46, 32))) {
                current_page_ = PanelPage::Vibrato;
                cursor_param_ = 0;
                update_lcd();
  }
  CzTutorial::instance().tooltip("vibrato");
  ImGui::SameLine(0, 5);

  if (CasioButton("DETUNE##param", current_page_ == PanelPage::Detune, true,
                  ImVec2(56, 32))) {
                current_page_ = PanelPage::Detune;
                cursor_param_ = 0;
                update_lcd();
  }
  CzTutorial::instance().tooltip("detune");
  ImGui::SameLine(0, 12);

  // Line 1 vs Line 2 toggle
  ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "LINE:");
  ImGui::SameLine(0, 5);
                if (CasioButton("1##linesel", current_line_ == 1, true,
                                ImVec2(32, 32))) {
                  current_line_ = 1;
                  update_lcd();
                }
                CzTutorial::instance().tooltip("line_select");
                ImGui::SameLine(0, 5);
                  if (CasioButton("2##linesel", current_line_ == 2, true,
                                  ImVec2(32, 32))) {
                    current_line_ = 2;
                    update_lcd();
                  }
                  CzTutorial::instance().tooltip("line_select");
                  ImGui::SameLine(0, 12);

                  // Envelope section selector
  if (CasioButton("DCO##param", current_page_ == PanelPage::Dco, true, ImVec2(46, 32),
                  IM_COL32(32, 210, 255, 255))) {
                    current_page_ = PanelPage::Dco;
                    envelope_graph_.set_active_section(0);
                    cursor_param_ = 0;
                    update_lcd();
  }
  CzTutorial::instance().tooltip("dco");
  ImGui::SameLine(0, 5);

  if (CasioButton("DCW##param", current_page_ == PanelPage::Dcw, true, ImVec2(46, 32),
                  IM_COL32(255, 175, 32, 255))) {
                    current_page_ = PanelPage::Dcw;
                    envelope_graph_.set_active_section(1);
                    cursor_param_ = 0;
                    update_lcd();
  }
  CzTutorial::instance().tooltip("dcw");
  ImGui::SameLine(0, 5);

  if (CasioButton("DCA##param", current_page_ == PanelPage::Dca, true, ImVec2(46, 32),
                  IM_COL32(64, 255, 96, 255))) {
                    current_page_ = PanelPage::Dca;
                    envelope_graph_.set_active_section(2);
                    cursor_param_ = 0;
                    update_lcd();
  }
  CzTutorial::instance().tooltip("dca");
  ImGui::PopID();
                }

                void CzPanel::render_bank_number_buttons() {
                  ImGui::PushID("BankDeck");
                  ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "BANCO:");
                  ImGui::SameLine(0, 5);

                  static const char banks[] = {'A', 'B', 'C', 'D'};
                  char cur_b = edit_patch_.memory.bank;
                  int cur_s = edit_patch_.memory.slot;

                  for (int b = 0; b < 4; ++b) {
                    char label[16];
                    snprintf(label, sizeof(label), "%c##bank", banks[b]);
                    if (CasioButton(label, cur_b == banks[b], true,
                                    ImVec2(32, 30),
                                    IM_COL32(255, 190, 0, 255))) {
                      select_patch(CzBank::get_index(banks[b], cur_s));
                    }
                    ImGui::SameLine(0, 4);
                  }

                  ImGui::SameLine(0, 12);
                  ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "NUMERO:");
                  ImGui::SameLine(0, 5);

                  for (int s = 1; s <= 8; ++s) {
                    char label[16];
                    snprintf(label, sizeof(label), "%d##num", s);
                    if (CasioButton(label, cur_s == s, true, ImVec2(28, 30),
                                    IM_COL32(255, 60, 60, 255))) {
                      select_patch(CzBank::get_index(cur_b, s));
                    }
                    if (s < 8) {
                      ImGui::SameLine(0, 4);
                    }
                  }
                  ImGui::PopID();
                }

                void CzPanel::render_audition_keyboard() {
                  ImGui::PushID("AuditionKbd");
                  ImDrawList *draw = ImGui::GetWindowDrawList();

                  ImGui::TextColored(
                      ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                      "TECLADO DE AUDICION EN VIVO (6 OCTAVAS: C1 - C7):");

                  ImVec2 k_pos = ImGui::GetCursorScreenPos();
                  float avail_w = ImGui::GetContentRegionAvail().x;
                  // 6 octaves (C1 to C7) = 43 white keys (6 * 7 + 1)
                  const int NUM_WHITE_KEYS = 43;
                  float white_w = std::floor(avail_w / static_cast<float>(NUM_WHITE_KEYS));
                  if (white_w < 16.0f)
                    white_w = 16.0f;
                  if (white_w > 26.0f)
                    white_w = 26.0f;

                  const float white_h = 95.0f;
                  const float black_w = std::floor(white_w * 0.62f);
                  const float black_h = std::floor(white_h * 0.62f);

                  static const int white_offsets[7] = {0, 2, 4, 5, 7, 9, 11};
                  static const bool has_black[7] = {true, true, false, true, true, true, false};
                  static const int black_offsets[7] = {1, 3, -1, 6, 8, 10, -1};

                  int clicked_note = -1;

                  // 1. Draw White Keys
                  for (int w = 0; w < NUM_WHITE_KEYS; ++w) {
                    int oct = w / 7;
                    int sub = w % 7;
                    int note = 24 + (oct * 12) + white_offsets[sub];

                    ImVec2 p0 = ImVec2(k_pos.x + (w * white_w), k_pos.y);
                    ImVec2 p1 = ImVec2(p0.x + white_w, p0.y + white_h);

                    bool active = (active_piano_key_ == note);
                    draw->AddRectFilled(p0, p1,
                                        active ? IM_COL32(200, 225, 255, 255)
                                               : IM_COL32(248, 248, 248, 255),
                                        2.0f);
                    draw->AddRect(p0, p1, IM_COL32(50, 50, 50, 255), 2.0f, 0,
                                  1.0f);

                    // Draw note label on C keys (C1, C2, ... C7)
                    if (sub == 0) {
                      char c_label[8];
                      snprintf(c_label, sizeof(c_label), "C%d", oct + 1);
                      draw->AddText(ImVec2(p0.x + 2.0f, p1.y - 15.0f),
                                    IM_COL32(110, 110, 130, 255), c_label);
                    }

                    // Click detection
                    if (ImGui::IsMouseHoveringRect(p0, p1) &&
                        ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                      clicked_note = note;
                    }
                  }

                  // 2. Draw Black Keys on top
                  for (int w = 0; w < NUM_WHITE_KEYS - 1; ++w) {
                    int oct = w / 7;
                    int sub = w % 7;
                    if (!has_black[sub])
                      continue;

                    int note = 24 + (oct * 12) + black_offsets[sub];
                    ImVec2 p0 =
                        ImVec2(k_pos.x + ((w + 1) * white_w) - (black_w * 0.5f),
                               k_pos.y);
                    ImVec2 p1 = ImVec2(p0.x + black_w, p0.y + black_h);

                    bool active = (active_piano_key_ == note);
                    draw->AddRectFilled(p0, p1,
                                        active ? IM_COL32(70, 140, 250, 255)
                                               : IM_COL32(25, 25, 28, 255),
                                        1.5f);
                    draw->AddRect(p0, p1, IM_COL32(0, 0, 0, 255), 1.5f, 0,
                                  1.0f);

                    // Click detection (overrides white key underneath)
                    if (ImGui::IsMouseHoveringRect(p0, p1) &&
                        ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                      clicked_note = note;
                    }
                  }

                  // MIDI Note Triggering
                  if (clicked_note != active_piano_key_) {
                    if (active_piano_key_ != -1) {
                      midi_.send_note_off(
                          static_cast<uint8_t>(active_piano_key_),
                          edit_patch_.midi_channel);
                    }
                    if (clicked_note != -1) {
                      midi_.send_note_on(static_cast<uint8_t>(clicked_note),
                                         100, edit_patch_.midi_channel);
                    }
                    active_piano_key_ = clicked_note;
                  }

                  ImGui::Dummy(ImVec2(NUM_WHITE_KEYS * white_w, white_h + 10.0f));
                  ImGui::PopID();
                }

                void CzPanel::render_bottom_toolbar() {
                  ImGui::Spacing();
                  ImGui::Separator();

                  ImGui::TextColored(
                      ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "MIDI Port: %s | Out: %s",
                      midi_.is_input_open() ? "CONECTADO" : "DESCONECTADO",
                      midi_.is_output_open() ? "CONECTADO" : "DESCONECTADO");
                }

} // namespace cz