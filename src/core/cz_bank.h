#pragma once

#include "cz_types.h"
#include <vector>
#include <string>
#include <array>

namespace cz {

class CzBank {
public:
    static constexpr size_t BANK_SIZE = 32; // 4 banks * 8 slots for CZ-3000

    std::array<CzPatch, BANK_SIZE> patches;

    CzBank();

    CzPatch& get_patch(char bank, int slot);
    const CzPatch& get_patch(char bank, int slot) const;

    CzPatch& get_patch_by_index(size_t index);
    const CzPatch& get_patch_by_index(size_t index) const;

    void initialize_factory_presets();
    void reslot_all();

    static size_t get_index(char bank, int slot);
    static std::pair<char, int> get_bank_slot(size_t index);
};

} // namespace cz

