#include "cz_bank.h"
#include <algorithm>
#include <sstream>

namespace cz {

CzBank::CzBank() {
    initialize_factory_presets();
}

size_t CzBank::get_index(char bank, int slot) {
    int b = std::clamp(bank - 'A', 0, 3);
    int s = std::clamp(slot - 1, 0, 7);
    return static_cast<size_t>(b * 8 + s);
}

std::pair<char, int> CzBank::get_bank_slot(size_t index) {
    index = std::clamp(index, static_cast<size_t>(0), BANK_SIZE - 1);
    char bank = static_cast<char>('A' + (index / 8));
    int slot = static_cast<int>((index % 8) + 1);
    return {bank, slot};
}

CzPatch& CzBank::get_patch(char bank, int slot) {
    return patches[get_index(bank, slot)];
}

const CzPatch& CzBank::get_patch(char bank, int slot) const {
    return patches[get_index(bank, slot)];
}

CzPatch& CzBank::get_patch_by_index(size_t index) {
    return patches[std::clamp(index, static_cast<size_t>(0), BANK_SIZE - 1)];
}

const CzPatch& CzBank::get_patch_by_index(size_t index) const {
    return patches[std::clamp(index, static_cast<size_t>(0), BANK_SIZE - 1)];
}

void CzBank::initialize_factory_presets() {
    // Authentic Casio CZ-3000 factory patch names for reference
    static const char* preset_names[BANK_SIZE] = {
        // Bank A
        "BRASS ENSEMBLE 1", "TRUMPET", "VIOLIN 1", "STRING ENSEMBLE",
        "ELEC. PIANO 1", "ELEC. ORGAN 1", "FLUTE", "SYNTH LEAD 1",
        // Bank B
        "BRASS ENSEMBLE 2", "SYNTH CLAVI", "CELLO", "SYNTH STRINGS",
        "ELEC. PIANO 2", "JAZZ ORGAN", "CLARINET", "SYNTH BASS 1",
        // Bank C
        "SYNTH BRASS 1", "WARM PAD", "ACCORDION", "MARIMBA",
        "VIBRAPHONE", "CHURCH ORGAN", "WHISTLE", "SYNTH LEAD 2",
        // Bank D
        "SYNTH BRASS 2", "SPACE FANTASY", "ACOUSTIC GUITAR", "ELEC. GUITAR",
        "SYNTH PLUCK", "FUNK BASS", "PERC. BELLS", "SYNTH FX 1"
    };

    static const char banks[] = {'A', 'B', 'C', 'D'};
    for (size_t i = 0; i < BANK_SIZE; ++i) {
        char b = banks[i / 8];
        int s = static_cast<int>((i % 8) + 1);
        patches[i] = CzPatch::create_default_patch(b, s, preset_names[i]);

        // Some realistic variation for defaults
        if (i % 2 == 1) {
            // Unison 1+1' detuned lead
            patches[i].line_select = LineSelect::Line1_Plus_1Prime;
            patches[i].detune.fine = 4;
        } else {
            patches[i].line_select = LineSelect::Line1_Plus_2Prime;
        }
    }
}

void CzBank::reslot_all() {
    static const char banks[] = {'A', 'B', 'C', 'D'};
    for (size_t i = 0; i < BANK_SIZE; ++i) {
        patches[i].memory.location = "internal";
        patches[i].memory.bank = banks[i / 8];
        patches[i].memory.slot = static_cast<int>((i % 8) + 1);
    }
}

} // namespace cz

