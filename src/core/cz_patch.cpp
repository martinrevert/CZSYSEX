#include "cz_types.h"
#include <algorithm>
#include <random>

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

void CzPatch::randomize() {
    static std::mt19937 rng(std::random_device{}());
    auto ri = [&](int lo, int hi) -> int {
        std::uniform_int_distribution<int> dist(lo, hi);
        return dist(rng);
    };

    static const LineSelect kLineModes[4] = {
        LineSelect::Line1,
        LineSelect::Line2,
        LineSelect::Line1_Plus_1Prime,
        LineSelect::Line1_Plus_2Prime
    };
    line_select = kLineModes[ri(0, 3)];
    octave = ri(-1, 1);

    detune.negative = (ri(0, 1) == 1);
    detune.octave = ri(0, 3);
    detune.note = ri(0, 11);
    detune.fine = ri(0, 60);

    vibrato.wave = ri(1, 4);
    vibrato.delay = ri(0, 99);
    vibrato.rate = ri(0, 99);
    vibrato.depth = ri(0, 99);

    auto randomize_line = [&](CzLine& line, bool allow_mod) {
        line.wave_1 = static_cast<uint8_t>(ri(0, 7));
        line.use_wave_2 = (ri(0, 1) == 1);
        line.wave_2 = line.use_wave_2 ? static_cast<uint8_t>(ri(0, 7)) : line.wave_1;
        line.window = static_cast<uint8_t>(ri(0, 7));
        line.modulation = allow_mod ? static_cast<uint8_t>(ri(0, 2)) : 0;

        // DCO envelope
        line.dco.end_step = ri(1, 8);
        line.dco.sustain_step = (ri(0, 3) == 0) ? 0 : ri(1, line.dco.end_step);
        for (int i = 0; i < 8; ++i) {
            line.dco.steps[i].rate = static_cast<uint8_t>(ri(0, 99));
            line.dco.steps[i].level = static_cast<uint8_t>(ri(0, 99));
        }

        // DCW envelope
        line.dcw.key_follow = ri(0, 9);
        line.dcw.end_step = ri(2, 8);
        line.dcw.sustain_step = (ri(0, 4) == 0) ? 0 : ri(1, line.dcw.end_step - 1);
        for (int i = 0; i < 8; ++i) {
            line.dcw.steps[i].rate = static_cast<uint8_t>(ri(0, 99));
            line.dcw.steps[i].level = static_cast<uint8_t>(ri(0, 99));
        }

        // DCA envelope (keep audible attack and zero at end step for clean note-off)
        line.dca.key_follow = ri(0, 9);
        line.dca.line_level = ri(8, 15);
        line.dca.end_step = ri(2, 8);
        line.dca.sustain_step = (ri(0, 4) == 0) ? 0 : ri(1, line.dca.end_step - 1);
        for (int i = 0; i < 8; ++i) {
            line.dca.steps[i].rate = static_cast<uint8_t>(ri(15, 99));
            line.dca.steps[i].level = static_cast<uint8_t>(ri(0, 99));
        }
        line.dca.steps[0].level = static_cast<uint8_t>(std::max<int>(line.dca.steps[0].level, 65));
        line.dca.steps[line.dca.end_step - 1].level = 0;
    };

    randomize_line(line1, true);
    randomize_line(line2, false);
}

} // namespace cz

