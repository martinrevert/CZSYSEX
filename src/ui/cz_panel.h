#pragma once

#include "../core/cz_bank.h"
#include "../midi/cz_midi.h"
#include "cz_lcd.h"
#include "cz_envelope_graph.h"
#include "cz_librarian.h"
#include "imgui.h"

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
    void select_patch(size_t index);

    CzBank& bank_;
    CzMidiManager& midi_;

    CzLcd lcd_;
    CzEnvelopeGraph envelope_graph_;
    CzLibrarian librarian_;

    size_t active_patch_index_ = 0;
    CzPatch edit_patch_;
    CzPatch compare_patch_;
    bool compare_active_ = false;

    PanelPage current_page_ = PanelPage::Play;
    int current_line_ = 1;      // 1 or 2
    int cursor_param_ = 0;      // 0 to N depending on page
    int active_step_ = 1;       // 1 to 8

    // UI state
    bool show_envelope_graph_ = true;
    bool show_librarian_ = true;
    bool show_tutorial_window_ = false;
    bool show_midi_settings_ = false;
    float master_volume_ = 0.85f;

    // Audition keyboard state
    int active_piano_key_ = -1;
};

} // namespace cz

