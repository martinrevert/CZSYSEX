#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "cz_panel.h"
#include "../core/cz_sysex.h"
#include "../core/cz_i18n.h"
#include "cz_tutorial.h"
#include "cz_manual.h"
#include <windows.h>
#include <commdlg.h>
#include <algorithm>
#include <cmath>
#include <cstdio>

#pragma comment(lib, "comdlg32.lib")

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
  std::string cache_path = get_presets_cache_path();
  if (!cache_path.empty() && bank_.load_presets_cache(cache_path)) {
    librarian_.set_status_message("Caché de 32 Presets cargada.");
  }
  select_patch(BankMode::Preset, 0, false);
}

std::string CzPanel::get_presets_cache_path() const {
  char local_app_data[MAX_PATH] = {0};
  DWORD len = GetEnvironmentVariableA("LOCALAPPDATA", local_app_data, MAX_PATH);
  if (len == 0 || len >= MAX_PATH) {
    return "presets_cache.syx";
  }
  std::string dir = std::string(local_app_data) + "\\CZ3000SysEx";
  CreateDirectoryA(dir.c_str(), nullptr);
  return dir + "\\presets_cache.syx";
}

void CzPanel::select_patch(BankMode mode, size_t index, bool send_midi) {
  active_bank_mode_ = mode;
  active_patch_index_ =
      std::clamp(index, static_cast<size_t>(0), CzBank::BANK_SIZE - 1);

  original_patch_ = bank_.get_patch_by_index(mode, active_patch_index_);
  edit_patch_ = original_patch_;
  edit_patch_.midi_channel = midi_channel_;
  compare_active_ = false;
  current_page_ = PanelPage::Play;

  if (send_midi && midi_.is_output_open()) {
    // Official CZ-3000 MIDI Implementation (Manual p. 41):
    // PC 0..31 = PRESET A-1..D-8, PC 32..63 = MEMORY A-1..D-8
    uint8_t pc = (mode == BankMode::Preset)
                     ? static_cast<uint8_t>(active_patch_index_)
                     : static_cast<uint8_t>(32 + active_patch_index_);
    midi_.send_program_change(pc, midi_channel_);

    if (mode == BankMode::Memory && live_audition_sync_) {
      send_edit_to_recall_buffer();
    } else if (auto_fetch_on_select_ && !batch_active_ && midi_.is_input_open()) {
      request_slot_dump(mode, active_patch_index_);
    }
  }

  update_lcd();
}

void CzPanel::request_slot_dump(BankMode mode, size_t index) {
  if (!midi_.is_input_open() || !midi_.is_output_open()) {
    librarian_.set_status_message("Conecta MIDI In y MIDI Out para leer del CZ-3000.");
    return;
  }
  size_t clamped = std::clamp(index, static_cast<size_t>(0), CzBank::BANK_SIZE - 1);
  uint8_t slot_byte = (mode == BankMode::Preset)
                          ? static_cast<uint8_t>(clamped & 0x1F)
                          : static_cast<uint8_t>(0x20 + (clamped & 0x1F));
  pending_request_active_ = true;
  pending_request_mode_ = mode;
  pending_request_index_ = clamped;
  pending_request_time_ = std::chrono::steady_clock::now();
  if (!midi_.request_patch_dump(midi_channel_, slot_byte)) {
    pending_request_active_ = false;
    librarian_.set_status_message("Error al solicitar SysEx: " + midi_.get_last_error());
  }
}

void CzPanel::send_edit_to_recall_buffer() {
  if (!midi_.is_output_open())
    return;
  CzPatch audition = edit_patch_;
  audition.midi_channel = midi_channel_;
  audition.memory.location = "recall"; // 0x60 COMPARE/RECALL buffer
  auto syx = CzSysEx::patch_to_sysex(audition, true);
  midi_.send_sysex(syx);
}

void CzPanel::process_incoming_sysex() {
  std::vector<uint8_t> msg;
  while (midi_.poll_sysex(msg)) {
    rx_sysex_count_++;
    rx_last_bytes_ = msg.size();

    // Casio CZ Handshake: if synth sends short 0x30 "ready to send", reply 0x31 ACK
    if (msg.size() == 7 && msg[0] == 0xF0 && msg[1] == 0x44 && msg[5] == 0x30 &&
        msg[6] == 0xF7) {
      std::vector<uint8_t> ack = {0xF0, 0x44, 0x00, 0x00, msg[4], 0x31, 0xF7};
      midi_.send_sysex(ack);
      continue;
    }

    CzPatch rx_patch;
    if (!CzSysEx::sysex_to_patch(msg, rx_patch)) {
      continue;
    }

    // Expire stale single request (> 2 seconds)
    if (pending_request_active_) {
      auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::steady_clock::now() - pending_request_time_)
                     .count();
      if (age > 2000) {
        pending_request_active_ = false;
      }
    }

    // CZ-3000 dump responses to SEND REQUEST (10 <slot>) are 263 bytes (no slot byte),
    // which CzSysEx::sysex_to_patch maps to "recall". Map them to the requested Preset/Memory slot!
    if (rx_patch.memory.location == "recall" &&
        (batch_active_ || pending_request_active_)) {
      BankMode target_mode = batch_active_ ? batch_mode_ : pending_request_mode_;
      size_t target_idx = batch_active_ ? batch_next_index_ : pending_request_index_;
      auto [b, s] = CzBank::get_bank_slot(target_idx);
      rx_patch.memory.location = (target_mode == BankMode::Preset) ? "preset" : "internal";
      rx_patch.memory.bank = b;
      rx_patch.memory.slot = s;
      pending_request_active_ = false;
    }

    if (rx_patch.memory.location == "recall") {
      edit_patch_ = rx_patch;
      edit_patch_.midi_channel = midi_channel_;
      compare_active_ = true;
      librarian_.set_status_message("Recibido búfer COMPARE/RECALL (0x60) del CZ-3000.");
      update_lcd();
    } else if (rx_patch.memory.location == "preset") {
      size_t idx = CzBank::get_index(rx_patch.memory.bank, rx_patch.memory.slot);
      // Preserve official preset name if incoming SysEx is 264-byte simple format
      if (rx_patch.simple_sysex || rx_patch.name == "CZ3000 SOUND" || rx_patch.name.empty()) {
        rx_patch.name = bank_.preset_patches[idx].name;
      }
      bank_.preset_patches[idx] = rx_patch;
      if (!batch_active_) {
        bank_.save_presets_cache(get_presets_cache_path());
      }
      if (active_bank_mode_ == BankMode::Preset && active_patch_index_ == idx) {
        original_patch_ = rx_patch;
        edit_patch_ = rx_patch;
        compare_active_ = false;
        update_lcd();
      }
      if (batch_active_ && batch_mode_ == BankMode::Preset &&
          batch_waiting_response_ && idx == batch_next_index_) {
        batch_received_count_++;
        batch_next_index_++;
        batch_waiting_response_ = false;
      } else {
        librarian_.set_status_message(
            "Recibido PRESET " + std::string(1, rx_patch.memory.bank) + "-" +
            std::to_string(rx_patch.memory.slot) + " del CZ-3000.");
      }
    } else {
      size_t idx = CzBank::get_index(rx_patch.memory.bank, rx_patch.memory.slot);
      rx_patch.memory.location = "internal";
      // Preserve local *MEMORY* or custom name when receiving 264-byte simple SysEx from CZ-3000
      if (rx_patch.simple_sysex || rx_patch.name == "CZ3000 SOUND" || rx_patch.name.empty()) {
        rx_patch.name = bank_.patches[idx].name;
      }
      bank_.patches[idx] = rx_patch;
      if (active_bank_mode_ == BankMode::Memory && active_patch_index_ == idx) {
        original_patch_ = rx_patch;
        edit_patch_ = rx_patch;
        compare_active_ = false;
        update_lcd();
      }
      if (batch_active_ && batch_mode_ == BankMode::Memory &&
          batch_waiting_response_ && idx == batch_next_index_) {
        batch_received_count_++;
        batch_next_index_++;
        batch_waiting_response_ = false;
      } else {
        librarian_.set_status_message(
            "Recibida MEMORY " + std::string(1, rx_patch.memory.bank) + "-" +
            std::to_string(rx_patch.memory.slot) + " del CZ-3000.");
      }
    }
  }
}

void CzPanel::start_batch_download(BankMode mode) {
  if (!midi_.is_input_open() || !midi_.is_output_open()) {
    librarian_.set_status_message("Conecta MIDI In y MIDI Out para descargar del CZ-3000.");
    return;
  }
  batch_active_ = true;
  batch_mode_ = mode;
  batch_next_index_ = 0;
  batch_waiting_response_ = false;
  batch_received_count_ = 0;
  batch_step_start_ = std::chrono::steady_clock::now();
}

void CzPanel::process_batch_dump() {
  if (!batch_active_) {
    return;
  }

  auto now = std::chrono::steady_clock::now();
  if (batch_waiting_response_) {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - batch_step_start_).count();
    if (elapsed >= 750) {
      // Timeout for this slot, move to next
      batch_next_index_++;
      batch_waiting_response_ = false;
    }
  }

  if (!batch_waiting_response_) {
    if (batch_next_index_ >= CzBank::BANK_SIZE) {
      batch_active_ = false;
      if (batch_mode_ == BankMode::Preset) {
        if (batch_received_count_ == static_cast<int>(CzBank::BANK_SIZE)) {
          bank_.set_presets_cached(true);
        }
        if (batch_received_count_ > 0) {
          bank_.save_presets_cache(get_presets_cache_path());
        }
        librarian_.set_status_message(
            "Descarga de Presets completada: " + std::to_string(batch_received_count_) +
            "/32 recibidos (caché guardada).");
      } else {
        librarian_.set_status_message(
            "Descarga de Memorias completada: " + std::to_string(batch_received_count_) +
            "/32 recibidas.");
      }
      select_patch(active_bank_mode_, active_patch_index_, false);
      return;
    }

    auto [b, s] = CzBank::get_bank_slot(batch_next_index_);
    uint8_t slot_byte = (batch_mode_ == BankMode::Preset)
                            ? static_cast<uint8_t>(batch_next_index_ & 0x1F)
                            : static_cast<uint8_t>(0x20 + (batch_next_index_ & 0x1F));
    midi_.request_patch_dump(midi_channel_, slot_byte);
    batch_waiting_response_ = true;
    batch_step_start_ = now;
    librarian_.set_status_message(
        std::string(batch_mode_ == BankMode::Preset ? "Descargando PRESET " : "Descargando MEMORY ") +
        std::string(1, b) + "-" + std::to_string(s) + " (" +
        std::to_string(batch_next_index_ + 1) + "/32)...");
  }
}

void CzPanel::load_sysex_file_dialog() {
  char file_buf[MAX_PATH] = {0};
  OPENFILENAMEA ofn{};
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFilter = "Casio CZ SysEx (*.syx)\0*.syx\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = file_buf;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
  ofn.lpstrDefExt = "syx";

  if (!GetOpenFileNameA(&ofn)) {
    return;
  }

  std::vector<CzPatch> loaded;
  std::string err_msg;
  if (!CzSysEx::load_syx_file(file_buf, loaded, err_msg) || loaded.empty()) {
    librarian_.set_status_message(err_msg.empty() ? "No se encontraron parches CZ válidos en el archivo." : err_msg);
    return;
  }

  if (loaded.size() == 1) {
    CzPatch p = loaded[0];
    auto [b, s] = CzBank::get_bank_slot(active_patch_index_);
    p.memory.location = "internal";
    p.memory.bank = b;
    p.memory.slot = s;
    bank_.patches[active_patch_index_] = p;
    select_patch(BankMode::Memory, active_patch_index_, true);
    send_edit_to_recall_buffer();
    librarian_.set_status_message("Parche .syx cargado en MEMORY " + std::string(1, b) + "-" + std::to_string(s) + ".");
  } else {
    size_t count = (std::min)(loaded.size(), CzBank::BANK_SIZE);
    for (size_t i = 0; i < count; ++i) {
      auto [b, s] = CzBank::get_bank_slot(i);
      bank_.patches[i] = loaded[i];
      bank_.patches[i].memory.location = "internal";
      bank_.patches[i].memory.bank = b;
      bank_.patches[i].memory.slot = s;
    }
    select_patch(BankMode::Memory, 0, true);
    librarian_.set_status_message("Banco de " + std::to_string(count) + " parches cargado en MEMORY BANK.");
  }
}

void CzPanel::save_sysex_file_dialog() {
  char file_buf[MAX_PATH] = "cz3000_memory_bank.syx";
  OPENFILENAMEA ofn{};
  ofn.lStructSize = sizeof(ofn);
  ofn.lpstrFilter = "Casio CZ SysEx (*.syx)\0*.syx\0All Files (*.*)\0*.*\0";
  ofn.lpstrFile = file_buf;
  ofn.nMaxFile = MAX_PATH;
  ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
  ofn.lpstrDefExt = "syx";

  if (!GetSaveFileNameA(&ofn)) {
    return;
  }

  std::vector<CzPatch> to_save(bank_.patches.begin(), bank_.patches.end());
  // Save as CZ-1 SysEx (simple = false, 296 bytes) so custom patch names are preserved in the .syx file,
  // while live MIDI transfers to the CZ-3000 still use 264-byte simple SysEx.
  if (CzSysEx::save_syx_file(file_buf, to_save, true, false)) {
    librarian_.set_status_message("Banco de 32 memorias guardado en .syx (con nombres CZ-1).");
  } else {
    librarian_.set_status_message("Error al guardar el archivo .syx.");
  }
}

void CzPanel::update_lcd() {
  char l1[32];
  char l2[32];

  const CzPatch &p = compare_active_ ? edit_patch_ : original_patch_;
  const CzLine &line = (current_line_ == 1) ? p.line1 : p.line2;

  switch (current_page_) {
  case PanelPage::Play: {
    const char *mode_tag = compare_active_
                               ? "*RECALL*"
                               : (active_bank_mode_ == BankMode::Preset ? "*PRESET*" : "*MEMORY*");
    snprintf(l1, sizeof(l1), "%-8s %c-%d    ", mode_tag, p.memory.bank, p.memory.slot);
    snprintf(l2, sizeof(l2), "%-16.16s", p.name.c_str());
    lcd_.set_cursor(0, -1, false);
    break;
  }
  case PanelPage::Dco: {
    const auto &step = line.dco.steps[active_step_ - 1];
    const char *flag = (line.dco.sustain_step == active_step_)
                           ? "SUS"
                           : (line.dco.end_step == active_step_ ? "END" : "   ");
    snprintf(l1, sizeof(l1), "PITCH STEP%d %s ", active_step_, flag);
    snprintf(l2, sizeof(l2), "RATE=%02d LEVEL=%02d", step.rate, step.level);
    lcd_.set_cursor(1, (cursor_param_ == 0) ? 5 : 14, true);
    break;
  }
  case PanelPage::Dcw: {
    const auto &step = line.dcw.steps[active_step_ - 1];
    const char *flag = (line.dcw.sustain_step == active_step_)
                           ? "SUS"
                           : (line.dcw.end_step == active_step_ ? "END" : "   ");
    snprintf(l1, sizeof(l1), "WAVE  STEP%d %s ", active_step_, flag);
    snprintf(l2, sizeof(l2), "RATE=%02d LEVEL=%02d", step.rate, step.level);
    lcd_.set_cursor(1, (cursor_param_ == 0) ? 5 : 14, true);
    break;
  }
  case PanelPage::Dca: {
    const auto &step = line.dca.steps[active_step_ - 1];
    const char *flag = (line.dca.sustain_step == active_step_)
                           ? "SUS"
                           : (line.dca.end_step == active_step_ ? "END" : "   ");
    snprintf(l1, sizeof(l1), "AMP   STEP%d %s ", active_step_, flag);
    snprintf(l2, sizeof(l2), "RATE=%02d LEVEL=%02d", step.rate, step.level);
    lcd_.set_cursor(1, (cursor_param_ == 0) ? 5 : 14, true);
    break;
  }
  case PanelPage::Vibrato: {
    snprintf(l1, sizeof(l1), "WAVE=%d  DELAY=%02d", p.vibrato.wave,
             p.vibrato.delay);
    snprintf(l2, sizeof(l2), "RATE=%02d DEPTH=%02d", p.vibrato.rate,
             p.vibrato.depth);
    lcd_.set_cursor((cursor_param_ < 2) ? 0 : 1,
                    (cursor_param_ % 2 == 0) ? 5 : 14, true);
    break;
  }
  case PanelPage::Detune: {
    snprintf(l1, sizeof(l1), "DETUNE(%c)  OCT=%d",
             p.detune.negative ? '-' : '+', p.detune.octave);
    snprintf(l2, sizeof(l2), "NOTE=%02d  FINE=%02d", p.detune.note,
             p.detune.fine);
    lcd_.set_cursor((cursor_param_ < 2) ? 0 : 1,
                    (cursor_param_ % 2 == 0) ? 5 : 14, true);
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
  if (current_page_ == PanelPage::Play) {
    int new_idx = static_cast<int>(active_patch_index_) + delta;
    if (new_idx >= 0 && new_idx < static_cast<int>(CzBank::BANK_SIZE)) {
      select_patch(active_bank_mode_, static_cast<size_t>(new_idx), true);
    }
    return;
  }

  CzLine &line = (current_line_ == 1) ? edit_patch_.line1 : edit_patch_.line2;

  switch (current_page_) {
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
  case PanelPage::Play:
  case PanelPage::Status:
    break;
  }

  compare_active_ = true;
  if (active_bank_mode_ == BankMode::Memory) {
    bank_.patches[active_patch_index_] = edit_patch_;
  }
  send_edit_to_recall_buffer();
  update_lcd();
}

void CzPanel::render() {
  process_incoming_sysex();
  process_batch_dump();
  render_header_bar();
  render_hardware_chassis();

  if (show_envelope_graph_) {
    ImGui::Spacing();
    ImGui::Separator();
    if (envelope_graph_.render(edit_patch_, current_line_)) {
      compare_active_ = true;
      if (active_bank_mode_ == BankMode::Memory) {
        bank_.patches[active_patch_index_] = edit_patch_;
      }
      send_edit_to_recall_buffer();
      update_lcd();
    }
  }

  render_bottom_toolbar();

  // Tutorial modal guide if requested
  if (show_tutorial_window_) {
    CzTutorial::instance().render_guide_window(&show_tutorial_window_);
  }

  // Official CZ-3000 Operation Manual if requested
  if (show_manual_window_) {
    CzManual::instance().render_manual_window(&show_manual_window_);
  }
}

void CzPanel::render_header_bar() {
  ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(22, 25, 28, 255));
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu(tr("menu.file"))) {
      if (ImGui::MenuItem(tr("menu.load_syx"))) {
        load_sysex_file_dialog();
      }
      if (ImGui::MenuItem(tr("menu.save_syx"))) {
        save_sysex_file_dialog();
      }
      ImGui::Separator();
      if (ImGui::MenuItem(tr("menu.reset_bank"))) {
        bank_.initialize_factory_presets();
        select_patch(active_bank_mode_, 0, true);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(tr("menu.views"))) {
      ImGui::MenuItem(tr("menu.env_graph"), nullptr, &show_envelope_graph_);
      ImGui::MenuItem(tr("menu.librarian"), nullptr,
                      &show_librarian_);
      ImGui::MenuItem(tr("menu.pd_guide"), nullptr,
                      &show_tutorial_window_);
      ImGui::MenuItem(tr("menu.manual"), nullptr,
                      &show_manual_window_);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(tr("menu.midi"))) {
      auto in_devs = midi_.get_input_devices();
      auto out_devs = midi_.get_output_devices();

      ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f),
                         "%s", tr("menu.midi_in"));
      for (const auto &dev : in_devs) {
        bool sel = (midi_.current_input_id() == dev.id);
        if (ImGui::MenuItem(dev.name.c_str(), nullptr, sel)) {
          midi_.open_input(dev.id);
        }
      }
      if (in_devs.empty())
        ImGui::TextDisabled("%s", tr("menu.no_midi_in"));

      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f),
                         "%s", tr("menu.midi_out"));
      for (const auto &dev : out_devs) {
        bool sel = (midi_.current_output_id() == dev.id);
        if (ImGui::MenuItem(dev.name.c_str(), nullptr, sel)) {
          midi_.open_output(dev.id);
        }
      }
      if (out_devs.empty())
        ImGui::TextDisabled("%s", tr("menu.no_midi_out"));

      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f),
                         "%s", tr("menu.midi_ch"));
      if (ImGui::BeginMenu(tr("menu.select_ch"))) {
        for (uint8_t ch = 1; ch <= 16; ++ch) {
          char ch_lbl[32];
          snprintf(ch_lbl, sizeof(ch_lbl), tr("menu.ch_prefix"), ch);
          if (ImGui::MenuItem(ch_lbl, nullptr, midi_channel_ == ch)) {
            midi_channel_ = ch;
            edit_patch_.midi_channel = ch;
          }
        }
        ImGui::EndMenu();
      }
      ImGui::TextDisabled("%s", tr("menu.cz_prg_hint"));

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu(tr("menu.language"))) {
      Language cur = CzI18n::instance().current_language();
      for (int i = 0; i < 8; ++i) {
        Language l = static_cast<Language>(i);
        if (ImGui::MenuItem(CzI18n::language_name(l), nullptr, cur == l)) {
          CzI18n::instance().set_language(l);
        }
      }
      ImGui::EndMenu();
    }

    // Right side prominent Tutorial button
    float tut_w = 240.0f;
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - tut_w - 10.0f);

    bool tut_on = CzTutorial::instance().is_enabled();
    if (tut_on) {
      ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 140, 150, 255));
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    } else {
      ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(60, 60, 60, 255));
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
    }

    if (ImGui::Button(tut_on ? tr("btn.tutorial_on") : tr("btn.tutorial_off"),
                      ImVec2(180, 20))) {
      CzTutorial::instance().toggle();
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    if (ImGui::SmallButton(tr("btn.guide"))) {
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
  float panel_h = 365.0f;

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
    float div_x = panel_pos.x + 605.0f;
    draw->AddLine(ImVec2(div_x, panel_pos.y + 48.0f),
                  ImVec2(div_x, panel_pos.y + panel_h - 10.0f),
                  IM_COL32(60, 68, 78, 255), 1.5f);

    float right_w = panel_w - 625.0f;
    ImGui::SetCursorScreenPos(ImVec2(div_x + 10.0f, panel_pos.y + 48.0f));
    ImGui::BeginChild("RightLibrarian", ImVec2(right_w, panel_h - 58.0f), false);
    LibrarianAction lib_act = librarian_.render(
        active_bank_mode_, active_patch_index_, midi_channel_,
        live_audition_sync_, auto_fetch_on_select_, batch_active_);
    switch (lib_act) {
    case LibrarianAction::PatchSelected:
      select_patch(active_bank_mode_, active_patch_index_, true);
      break;
    case LibrarianAction::RequestSingleDump:
      request_slot_dump(active_bank_mode_, active_patch_index_);
      break;
    case LibrarianAction::BatchDownloadPresets:
      start_batch_download(BankMode::Preset);
      break;
    case LibrarianAction::BatchDownloadMemories:
      start_batch_download(BankMode::Memory);
      break;
    case LibrarianAction::CancelBatch:
      batch_active_ = false;
      librarian_.set_status_message("Descarga cancelada.");
      break;
    case LibrarianAction::RandomizeMemory:
      original_patch_ = bank_.get_patch_by_index(BankMode::Memory, active_patch_index_);
      edit_patch_ = original_patch_;
      edit_patch_.midi_channel = midi_channel_;
      compare_active_ = false;
      if (midi_.is_output_open()) {
        send_edit_to_recall_buffer();
      }
      update_lcd();
      break;
    case LibrarianAction::None:
      break;
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

  if (CasioButton("COMPARE/RECALL##prog", compare_active_, true, ImVec2(118, 34),
                  IM_COL32(255, 180, 0, 255))) {
    compare_active_ = !compare_active_;
    if (midi_.is_output_open()) {
      if (compare_active_) {
        send_edit_to_recall_buffer();
      } else {
        uint8_t pc = (active_bank_mode_ == BankMode::Preset)
                         ? static_cast<uint8_t>(active_patch_index_)
                         : static_cast<uint8_t>(32 + active_patch_index_);
        midi_.send_program_change(pc, midi_channel_);
      }
    }
    update_lcd();
  }
  CzTutorial::instance().tooltip("sysex_io");

  if (CasioButton("INIT##prog", false, false, ImVec2(56, 34))) {
    auto [b, s] = CzBank::get_bank_slot(active_patch_index_);
    edit_patch_ = CzPatch::create_default_patch(b, s, "INIT SOUND");
    edit_patch_.midi_channel = midi_channel_;
    compare_active_ = true;
    send_edit_to_recall_buffer();
    librarian_.set_status_message("Parche inicializado (INITIALIZE) en búfer 0x60.");
    update_lcd();
  }
  CzTutorial::instance().tooltip("sysex_io");
  ImGui::SameLine();

  if (CasioButton("WRITE##prog", false, true, ImVec2(56, 34),
                  IM_COL32(255, 40, 40, 255))) {
    auto [b, s] = CzBank::get_bank_slot(active_patch_index_);
    edit_patch_.memory.location = "internal";
    edit_patch_.memory.bank = b;
    edit_patch_.memory.slot = s;
    edit_patch_.midi_channel = midi_channel_;
    bank_.patches[active_patch_index_] = edit_patch_;
    original_patch_ = edit_patch_;
    active_bank_mode_ = BankMode::Memory;
    compare_active_ = false;

    auto syx = CzSysEx::patch_to_sysex(edit_patch_, true);
    if (midi_.send_sysex(syx)) {
      midi_.send_program_change(static_cast<uint8_t>(32 + active_patch_index_), midi_channel_);
      librarian_.set_status_message(
          "Guardado en MEMORY " + std::string(1, b) + "-" + std::to_string(s) + " del CZ-3000.");
    } else {
      librarian_.set_status_message("Guardado localmente (MIDI Out desconectado).");
    }
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

  static const char banks[] = {'A', 'B', 'C', 'D'};
  char cur_b = edit_patch_.memory.bank;
  int cur_s = edit_patch_.memory.slot;

  // Row 1: PRESET BANK (A..D) and MEMORY BANK (A..D)
  ImGui::TextColored(ImVec4(0.35f, 0.9f, 0.65f, 1.0f), "PRESET:");
  ImGui::SameLine(0, 5);
  for (int b = 0; b < 4; ++b) {
    char label[24];
    snprintf(label, sizeof(label), "%c##presetbank", banks[b]);
    bool active = (active_bank_mode_ == BankMode::Preset && cur_b == banks[b]);
    if (CasioButton(label, active, true, ImVec2(28, 28),
                    IM_COL32(40, 230, 140, 255))) {
      select_patch(BankMode::Preset, CzBank::get_index(banks[b], cur_s), true);
    }
    ImGui::SameLine(0, 3);
  }

  ImGui::SameLine(0, 12);
  ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "MEMORY:");
  ImGui::SameLine(0, 5);
  for (int b = 0; b < 4; ++b) {
    char label[24];
    snprintf(label, sizeof(label), "%c##membank", banks[b]);
    bool active = (active_bank_mode_ == BankMode::Memory && cur_b == banks[b]);
    if (CasioButton(label, active, true, ImVec2(28, 28),
                    IM_COL32(255, 160, 40, 255))) {
      select_patch(BankMode::Memory, CzBank::get_index(banks[b], cur_s), true);
    }
    if (b < 3) ImGui::SameLine(0, 3);
  }

  // Row 2: VOICE SELECTORS (1..8)
  ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "VOICE: ");
  ImGui::SameLine(0, 5);
  for (int s = 1; s <= 8; ++s) {
    char label[16];
    snprintf(label, sizeof(label), "%d##num", s);
    if (CasioButton(label, cur_s == s, true, ImVec2(32, 28),
                    IM_COL32(255, 60, 60, 255))) {
      select_patch(active_bank_mode_, CzBank::get_index(cur_b, s), true);
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
                      ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", tr("kbd.title"));

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
                          midi_channel_);
                    }
                    if (clicked_note != -1) {
                      midi_.send_note_on(static_cast<uint8_t>(clicked_note),
                                         100, midi_channel_);
                    }
                    active_piano_key_ = clicked_note;
                  }

                  ImGui::Dummy(ImVec2(NUM_WHITE_KEYS * white_w, white_h + 10.0f));
                  ImGui::PopID();
                }

                void CzPanel::render_bottom_toolbar() {
                  ImGui::Spacing();
                  ImGui::Separator();

                  int pc_num = (active_bank_mode_ == BankMode::Preset)
                                   ? static_cast<int>(active_patch_index_)
                                   : static_cast<int>(32 + active_patch_index_);
                  ImGui::TextColored(
                      ImVec4(0.7f, 0.7f, 0.7f, 1.0f),
                      tr("status.bar"),
                      midi_.is_input_open() ? tr("status.connected") : tr("status.disconnected"),
                      midi_.is_output_open() ? tr("status.connected") : tr("status.disconnected"),
                      midi_channel_,
                      active_bank_mode_ == BankMode::Preset ? "PRESET" : "MEMORY",
                      edit_patch_.memory.bank, edit_patch_.memory.slot, pc_num,
                      rx_sysex_count_, rx_last_bytes_);
                }

} // namespace cz