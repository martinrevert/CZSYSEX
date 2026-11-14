#include "cz_bank.h"
#include "cz_sysex.h"
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

CzPatch& CzBank::get_patch(BankMode mode, char bank, int slot) {
    auto& arr = (mode == BankMode::Preset) ? preset_patches : patches;
    return arr[get_index(bank, slot)];
}

const CzPatch& CzBank::get_patch(BankMode mode, char bank, int slot) const {
    const auto& arr = (mode == BankMode::Preset) ? preset_patches : patches;
    return arr[get_index(bank, slot)];
}

CzPatch& CzBank::get_patch_by_index(size_t index) {
    return patches[std::clamp(index, static_cast<size_t>(0), BANK_SIZE - 1)];
}

const CzPatch& CzBank::get_patch_by_index(size_t index) const {
    return patches[std::clamp(index, static_cast<size_t>(0), BANK_SIZE - 1)];
}

CzPatch& CzBank::get_patch_by_index(BankMode mode, size_t index) {
    auto& arr = (mode == BankMode::Preset) ? preset_patches : patches;
    return arr[std::clamp(index, static_cast<size_t>(0), BANK_SIZE - 1)];
}

const CzPatch& CzBank::get_patch_by_index(BankMode mode, size_t index) const {
    const auto& arr = (mode == BankMode::Preset) ? preset_patches : patches;
    return arr[std::clamp(index, static_cast<size_t>(0), BANK_SIZE - 1)];
}

void CzBank::initialize_factory_presets() {
    // Official Casio CZ-3000 32 Preset Voice Table (Owner's Manual p. 11)
    static const char* official_preset_names[BANK_SIZE] = {
        // Bank A
        "BRASS ENSEMBLE 1", "STRING ENSEMBLE 1", "JAZZ ORGAN", "WHISTLE",
        "FAT BASS", "VIBRAPHONE", "SYNTH. DRUMS", "HUMAN VOICE",
        // Bank B
        "SYNTH. BRASS", "STRING ENSEMBLE 2", "ELEC. PIANO", "FLUTE",
        "SYNTH. BASS", "CRISPY XYLOPHONE", "STEEL DRUM", "FAIRY TALE",
        // Bank C
        "BRASS ENSEMBLE 2", "SYNTH. STRINGS", "ACCORDION", "BLUES HARMONICA",
        "ELEC. GUITAR", "SYNTH. GLOCKENSPIEL", "SYNTH. PERCUSSION", "CARILLON",
        // Bank D
        "TRUMPET", "VIOLIN", "FANTASTIC ORGAN", "DOUBLE REED",
        "METALLIC SOUND", "CARIMBA", "CONGA", "TYPHOON SOUND"
    };

    // Voices marked '*' in manual p. 11 are 16-tone polyphonic (1 DCO)
    static const bool is_1_dco[BANK_SIZE] = {
        false, false, false, true,  false, false, false, false, // A-4 WHISTLE *
        false, false, false, true,  false, false, false, false, // B-4 FLUTE *
        false, false, false, false, false, false, false, false, // Bank C
        false, true,  false, false, false, false, false, false  // D-2 VIOLIN *
    };

    static const char banks[] = {'A', 'B', 'C', 'D'};
    for (size_t i = 0; i < BANK_SIZE; ++i) {
        char b = banks[i / 8];
        int s = static_cast<int>((i % 8) + 1);

        // 1. Fixed Preset Bank (0..31)
        preset_patches[i] = CzPatch::create_default_patch(b, s, official_preset_names[i]);
        preset_patches[i].memory.location = "preset";
        preset_patches[i].line_select = is_1_dco[i] ? LineSelect::Line1 : LineSelect::Line1_Plus_2Prime;

        // 2. Editable User Memory Bank (32..63) initialized with authentic *MEMORY* A-1..D-8 labels
        std::string mem_name = "*MEMORY* " + std::string(1, b) + "-" + std::to_string(s);
        patches[i] = CzPatch::create_default_patch(b, s, mem_name);
        patches[i].memory.location = "internal";
        patches[i].line_select = LineSelect::Line1_Plus_2Prime;
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

bool CzBank::load_presets_cache(const std::string& filepath) {
    std::vector<CzPatch> loaded;
    std::string err;
    if (!CzSysEx::load_syx_file(filepath, loaded, err) || loaded.empty()) {
        return false;
    }
    static const char banks[] = {'A', 'B', 'C', 'D'};
    size_t count = (std::min)(loaded.size(), BANK_SIZE);
    for (size_t i = 0; i < count; ++i) {
        std::string official_name = preset_patches[i].name;
        preset_patches[i] = loaded[i];
        preset_patches[i].name = official_name; // Keep official CZ-3000 Preset name
        preset_patches[i].memory.location = "preset";
        preset_patches[i].memory.bank = banks[i / 8];
        preset_patches[i].memory.slot = static_cast<int>((i % 8) + 1);
    }
    presets_cached_ = (count == BANK_SIZE);
    return true;
}

bool CzBank::save_presets_cache(const std::string& filepath) const {
    std::vector<CzPatch> to_save(preset_patches.begin(), preset_patches.end());
    // Save without reslotting to internal so each preset keeps location="preset" (0x00..0x1F)
    return CzSysEx::save_syx_file(filepath, to_save, false, true);
}

} // namespace cz

