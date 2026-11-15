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

bool CzBank::is_slot_free(size_t index) const {
    if (index >= BANK_SIZE) {
        return false;
    }
    const std::string& name = patches[index].name;
    return name.rfind("*MEMORY*", 0) == 0 || name.rfind("[EMPTY]", 0) == 0;
}

size_t CzBank::count_free_slots() const {
    size_t free_count = 0;
    for (size_t i = 0; i < BANK_SIZE; ++i) {
        if (is_slot_free(i)) {
            ++free_count;
        }
    }
    return free_count;
}

SyxImportResult CzBank::import_syx_patches(const std::vector<CzPatch>& loaded,
                                           const std::string& file_stem,
                                           size_t preferred_start_index) {
    SyxImportResult result;
    std::vector<CzPatch> non_empty;
    non_empty.reserve(loaded.size());

    for (const auto& p : loaded) {
        if (p.name.rfind("[EMPTY]", 0) == 0) {
            ++result.skipped_empty_count;
        } else {
            non_empty.push_back(p);
        }
    }

    result.free_slots_before = count_free_slots();
    result.free_slots_after = result.free_slots_before;

    if (non_empty.empty()) {
        result.error_message = "El archivo .syx solo contiene parches vacíos [EMPTY].";
        return result;
    }

    if (non_empty.size() > result.free_slots_before) {
        if (result.free_slots_before == 0) {
            result.error_message =
                "Memoria de Usuario llena (32/32 ocupadas). No hay espacio para cargar " +
                std::to_string(non_empty.size()) +
                " parche(s). Usa File -> Reset Bank para liberar memoria.";
        } else {
            result.error_message =
                "Espacio insuficiente en Memoria de Usuario: el archivo tiene " +
                std::to_string(non_empty.size()) + " parche(s) y solo quedan " +
                std::to_string(result.free_slots_before) + " espacio(s) libre(s) de 32.";
        }
        return result;
    }

    size_t start = (preferred_start_index < BANK_SIZE) ? preferred_start_index : 0;
    std::vector<size_t> target_indices;
    target_indices.reserve(non_empty.size());

    for (size_t offset = 0; offset < BANK_SIZE && target_indices.size() < non_empty.size(); ++offset) {
        size_t idx = (start + offset) % BANK_SIZE;
        if (is_slot_free(idx)) {
            target_indices.push_back(idx);
        }
    }

    for (size_t i = 0; i < non_empty.size(); ++i) {
        size_t target_idx = target_indices[i];
        auto [b, s] = get_bank_slot(target_idx);
        patches[target_idx] = non_empty[i];
        patches[target_idx].memory.location = "internal";
        patches[target_idx].memory.bank = b;
        patches[target_idx].memory.slot = s;
        if (patches[target_idx].simple_sysex && !file_stem.empty()) {
            patches[target_idx].name = file_stem + " " + std::string(1, b) + "-" + std::to_string(s);
        }
    }

    result.success = true;
    result.imported_count = non_empty.size();
    result.first_imported_index = target_indices.front();
    result.free_slots_after = count_free_slots();
    return result;
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

