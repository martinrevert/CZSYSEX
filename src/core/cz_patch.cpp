#include "cz_types.h"

namespace cz {

CzLine::CzLine() {
    wave_1 = 0; // Sawtooth
    wave_2 = 0;
    use_wave_2 = false;
    window = 0;
    modulation = 0;

    // DCO envelope default
    dco.sustain_step = 1;
    dco.end_step = 8;
    for (int i = 0; i < 8; ++i) {
        dco.steps[i].rate = 50;
        dco.steps[i].level = 0;
    }

    // DCW envelope default
    dcw.sustain_step = 1;
    dcw.end_step = 8;
    dcw.steps[0].rate = 99;
    dcw.steps[0].level = 99;
    for (int i = 1; i < 8; ++i) {
        dcw.steps[i].rate = 50;
        dcw.steps[i].level = 0;
    }

    // DCA envelope default
    dca.sustain_step = 1;
    dca.end_step = 8;
    dca.line_level = 15;
    dca.steps[0].rate = 99;
    dca.steps[0].level = 99;
    for (int i = 1; i < 8; ++i) {
        dca.steps[i].rate = 50;
        dca.steps[i].level = 0;
    }
}

CzPatch::CzPatch() {
    name = "CZ3000 SOUND";
    midi_channel = 1;
    memory.location = "internal";
    memory.bank = 'A';
    memory.slot = 1;
    line_select = LineSelect::Line1_Plus_2Prime;
    octave = 0;
    simple_sysex = true;
}

CzPatch CzPatch::create_default_patch(char bank, int slot, const std::string& name) {
    CzPatch p;
    p.memory.bank = bank;
    p.memory.slot = slot;
    p.name = name;
    return p;
}

} // namespace cz

