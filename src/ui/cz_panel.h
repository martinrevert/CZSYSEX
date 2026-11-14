#pragma once

#include "../core/cz_bank.h"
#include "../midi/cz_midi.h"
#include "cz_lcd.h"
#include "cz_envelope_graph.h"
#include "cz_librarian.h"
#include "imgui.h"
#include <chrono>
#include <string>

namespace cz {

enum class PanelPage {
    Play,
    Dco,
    Dcw,
    Dca,
    Vibrato,
    Detune,
    LineSelect,
    Status
};

class CzPanel {
public:
    CzPanel(CzBank& bank, CzMidiManager& midi);

    // Renders the full CZ-3000 hardware simulation and interface
    void render();

private:
    void render_header_bar();
    void render_hardware_chassis();
    void render_programmer_section();
    void render_parameter_buttons();
    void render_data_entry_section();
    void render_bank_number_buttons();
    void render_audition_keyboard();
    void render_bottom_toolbar();

    void update_lcd();
    void increment_current_parameter(int delta);
    void select_patch(BankMode mode, size_t index, bool send_midi = true);
    void send_edit_to_recall_buffer();
    void request_slot_dump(BankMode mode, size_t index);
    void process_incoming_sysex();
    void start_batch_download(BankMode mode);
    void process_batch_dump();
    std::string get_presets_cache_path() const;
    void load_sysex_file_dialog();
    void save_sysex_file_dialog();

    CzBank& bank_;
    CzMidiManager& midi_;

    CzLcd lcd_;
    CzEnvelopeGraph envelope_graph_;
    CzLibrarian librarian_;

    BankMode active_bank_mode_ = BankMode::Preset;
    size_t active_patch_index_ = 0;
    uint8_t midi_channel_ = 1;
    bool live_audition_sync_ = false;
    bool auto_fetch_on_select_ = true;

    // Single / Batch SysEx request tracking (CZ-3000 replies with 263 bytes without slot byte)
    bool pending_request_active_ = false;
    BankMode pending_request_mode_ = BankMode::Preset;
    size_t pending_request_index_ = 0;
    std::chrono::steady_clock::time_point pending_request_time_{};

    // Non-blocking batch SysEx download state
    bool batch_active_ = false;
    BankMode batch_mode_ = BankMode::Preset;
    size_t batch_next_index_ = 0;
    bool batch_waiting_response_ = false;
    int batch_received_count_ = 0;
    std::chrono::steady_clock::time_point batch_step_start_{};

    CzPatch edit_patch_;
    CzPatch original_patch_;
    bool compare_active_ = false; // true = editing/auditioning COMPARE/RECALL (0x60) buffer

    PanelPage current_page_ = PanelPage::Play;
    int current_line_ = 1;      // 1 or 2
    int cursor_param_ = 0;      // 0 to N depending on page
    int active_step_ = 1;       // 1 to 8

    // UI state
    bool show_envelope_graph_ = true;
    bool show_librarian_ = true;
    bool show_tutorial_window_ = false;
    bool show_manual_window_ = false;
    bool show_midi_settings_ = false;
    float master_volume_ = 0.85f;

    // Audition keyboard state
    int active_piano_key_ = -1;

    // SysEx RX diagnostics
    int rx_sysex_count_ = 0;
    size_t rx_last_bytes_ = 0;
};

} // namespace cz

