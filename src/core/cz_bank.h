#pragma once

#include "cz_types.h"
#include <vector>
#include <string>
#include <array>

namespace cz {

enum class BankMode {
    Preset = 0, // 32 fixed ROM voices (MIDI PC 0-31, SysEx 0x00-0x1F)
    Memory = 1  // 32 user RAM voices (MIDI PC 32-63, SysEx 0x20-0x3F)
};

struct SyxImportResult {
    bool success = false;
    size_t imported_count = 0;
    size_t skipped_empty_count = 0;
    size_t free_slots_before = 0;
    size_t free_slots_after = 0;
    size_t first_imported_index = 0;
    std::string error_message;
};

class CzBank {
public:
    static constexpr size_t BANK_SIZE = 32; // 4 banks * 8 slots for CZ-3000

    std::array<CzPatch, BANK_SIZE> preset_patches;
    std::array<CzPatch, BANK_SIZE> patches; // User Memory Bank

    CzBank();

    CzPatch& get_patch(char bank, int slot);
    const CzPatch& get_patch(char bank, int slot) const;

    CzPatch& get_patch(BankMode mode, char bank, int slot);
    const CzPatch& get_patch(BankMode mode, char bank, int slot) const;

    CzPatch& get_patch_by_index(size_t index);
    const CzPatch& get_patch_by_index(size_t index) const;

    CzPatch& get_patch_by_index(BankMode mode, size_t index);
    const CzPatch& get_patch_by_index(BankMode mode, size_t index) const;

    void initialize_factory_presets();
    void reslot_all();

    bool is_slot_free(size_t index) const;
    size_t count_free_slots() const;
    SyxImportResult import_syx_patches(const std::vector<CzPatch>& loaded,
                                       const std::string& file_stem = "",
                                       size_t preferred_start_index = 0);

    bool load_presets_cache(const std::string& filepath);
    bool save_presets_cache(const std::string& filepath) const;
    bool is_presets_cached() const { return presets_cached_; }
    void set_presets_cached(bool cached) { presets_cached_ = cached; }

    bool load_memory_cache(const std::string& filepath);
    bool save_memory_cache(const std::string& filepath) const;

    static std::string get_appdata_dir();
    static std::string get_default_presets_cache_path();
    static std::string get_default_memory_cache_path();

    static size_t get_index(char bank, int slot);
    static std::pair<char, int> get_bank_slot(size_t index);

private:
    bool presets_cached_ = false;
};

} // namespace cz

