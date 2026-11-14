#include "../src/core/cz_math.h"
#include "../src/core/cz_types.h"
#include "../src/core/cz_sysex.h"
#include "../src/core/cz_bank.h"
#include <iostream>
#include <cassert>

using namespace cz;

int main() {
    std::cout << "[CZ-3000 C++] Running SysEx Core Tests...\n";

    // 1. Nibble packing
    for (int b = 0; b < 256; ++b) {
        auto [low, high] = CzMath::pack_casio(static_cast<uint8_t>(b));
        uint8_t unpacked = CzMath::unpack_casio(low, high);
        assert(unpacked == b);
    }
    std::cout << "  [PASS] Nibble pack/unpack 256 bytes.\n";

    // 2. Math rate/level roundtrips
    for (uint8_t val = 0; val <= 99; ++val) {
        uint8_t h_dca_r = CzMath::dca_rate_dec_to_hex(val);
        uint8_t d_dca_r = CzMath::dca_rate_hex_to_dec(h_dca_r);
        assert(std::abs(static_cast<int>(val) - static_cast<int>(d_dca_r)) <= 1);

        uint8_t h_dca_l = CzMath::dca_level_dec_to_hex(val);
        uint8_t d_dca_l = CzMath::dca_level_hex_to_dec(h_dca_l);
        assert(d_dca_l == val);

        uint8_t h_dcw_r = CzMath::dcw_rate_dec_to_hex(val);
        uint8_t d_dcw_r = CzMath::dcw_rate_hex_to_dec(h_dcw_r);
        assert(std::abs(static_cast<int>(val) - static_cast<int>(d_dcw_r)) <= 1);

        uint8_t h_dcw_l = CzMath::dcw_level_dec_to_hex(val);
        uint8_t d_dcw_l = CzMath::dcw_level_hex_to_dec(h_dcw_l);
        assert(std::abs(static_cast<int>(val) - static_cast<int>(d_dcw_l)) <= 1);

        uint8_t h_dco_r = CzMath::dco_rate_dec_to_hex(val);
        uint8_t d_dco_r = CzMath::dco_rate_hex_to_dec(h_dco_r);
        assert(std::abs(static_cast<int>(val) - static_cast<int>(d_dco_r)) <= 1);

        uint8_t h_dco_l = CzMath::dco_level_dec_to_hex(val);
        uint8_t d_dco_l = CzMath::dco_level_hex_to_dec(h_dco_l);
        assert(d_dco_l == val);
    }
    std::cout << "  [PASS] DCA/DCW/DCO rate/level mathematical mappings.\n";

    // 3. SysEx patch roundtrip
    CzPatch orig = CzPatch::create_default_patch('B', 4, "TEST LEAD");
    orig.octave = 1;
    orig.detune.negative = false;
    orig.detune.fine = 7;
    orig.detune.note = 0;
    orig.detune.octave = 0;
    orig.vibrato.delay = 15;
    orig.vibrato.rate = 65;
    orig.vibrato.depth = 22;
    orig.vibrato.wave = 3;
    orig.line1.wave_1 = 5; // Resonant 1
    orig.line1.wave_2 = 1; // Square
    orig.line1.use_wave_2 = true;
    orig.line1.dca.sustain_step = 3;
    orig.line1.dca.end_step = 6;
    orig.line1.dca.steps[0] = {99, 99};
    orig.line1.dca.steps[1] = {50, 70};
    orig.line1.dca.steps[2] = {40, 50}; // Sustain
    orig.line1.dca.steps[3] = {60, 0};  // Release

    std::vector<uint8_t> syx = CzSysEx::patch_to_sysex(orig, true); // CZ-3000 simple sysex
    assert(syx.size() == 264);
    assert(syx[0] == 0xF0);
    assert(syx[1] == 0x44);
    assert(syx[2] == 0x00);
    assert(syx[3] == 0x00);
    assert(syx.back() == 0xF7);

    CzPatch decoded;
    bool ok = CzSysEx::sysex_to_patch(syx, decoded);
    assert(ok);
    assert(decoded.memory.bank == 'B');
    assert(decoded.memory.slot == 4);
    assert(decoded.octave == 1);
    assert(decoded.detune.fine == 7);
    assert(decoded.vibrato.delay == 15);
    assert(decoded.vibrato.rate == 65);
    assert(decoded.vibrato.depth == 22);
    assert(decoded.vibrato.wave == 3);
    assert(decoded.line1.wave_1 == 5);
    assert(decoded.line1.wave_2 == 1);
    assert(decoded.line1.use_wave_2 == true);
    assert(decoded.line1.dca.sustain_step == 3);
    assert(decoded.line1.dca.end_step == 6);
    std::cout << "  [PASS] SysEx 264-byte CZ-3000 patch roundtrip.\n";

    // 4. Bank & Memory slot mapping tests (Official CZ-3000 Manual p. 11 & 31)
    assert(CzMath::memory_to_sysex("preset", 'A', 1, true) == 0x00);
    assert(CzMath::memory_to_sysex("preset", 'D', 8, true) == 0x1F);
    assert(CzMath::memory_to_sysex("internal", 'A', 1, true) == 0x20);
    assert(CzMath::memory_to_sysex("internal", 'D', 8, true) == 0x3F);
    assert(CzMath::memory_to_sysex("cartridge", 'A', 1, true) == 0x40);
    assert(CzMath::memory_to_sysex("recall", 'A', 1, true) == 0x60);

    std::string loc;
    char mbank = 'A';
    int mslot = 1;
    CzMath::memory_from_sysex(0x0B, true, loc, mbank, mslot); // Preset B-4 (FLUTE *)
    assert(loc == "preset" && mbank == 'B' && mslot == 4);
    CzMath::memory_from_sysex(0x20, true, loc, mbank, mslot); // Memory A-1
    assert(loc == "internal" && mbank == 'A' && mslot == 1);
    CzMath::memory_from_sysex(0x60, true, loc, mbank, mslot); // Compare/Recall
    assert(loc == "recall");
    std::cout << "  [PASS] Official CZ-3000 SysEx slot mappings (Preset/Memory/Cartridge/Recall).\n";

    CzBank bank;
    assert(bank.preset_patches.size() == 32);
    assert(bank.patches.size() == 32);
    assert(bank.get_patch(BankMode::Preset, 'A', 1).name == "BRASS ENSEMBLE 1");
    assert(bank.get_patch(BankMode::Preset, 'B', 4).name == "FLUTE");
    assert(bank.get_patch(BankMode::Preset, 'B', 4).line_select == LineSelect::Line1);
    assert(bank.get_patch(BankMode::Preset, 'D', 8).name == "TYPHOON SOUND");
    assert(bank.get_patch('A', 1).name == "*MEMORY* A-1");
    std::cout << "  [PASS] Dual 64-voice Bank (32 Preset + 32 Memory) initialization.\n";

    // 5. CZ-1 extended SysEx roundtrip (preserves custom patch names in .syx files)
    CzPatch named_patch = CzPatch::create_default_patch('C', 3, "MI SONIDO CZ3000");
    auto syx_cz1 = CzSysEx::patch_to_sysex(named_patch, false);
    assert(syx_cz1.size() == 296);
    CzPatch decoded_cz1;
    assert(CzSysEx::sysex_to_patch(syx_cz1, decoded_cz1));
    assert(decoded_cz1.name == "MI SONIDO CZ3000");
    assert(decoded_cz1.memory.bank == 'C' && decoded_cz1.memory.slot == 3);
    std::cout << "  [PASS] CZ-1 296-byte SysEx custom name preservation roundtrip.\n";

    // 6. Preset cache save/load roundtrip
    bank.preset_patches[3].vibrato.depth = 42;
    const std::string tmp_cache = "test_presets_cache.syx";
    assert(bank.save_presets_cache(tmp_cache));
    CzBank bank2;
    assert(bank2.load_presets_cache(tmp_cache));
    assert(bank2.is_presets_cached());
    assert(bank2.preset_patches[3].name == "WHISTLE");
    assert(bank2.preset_patches[3].vibrato.depth == 42);
    std::remove(tmp_cache.c_str());
    std::cout << "  [PASS] Preset bank cache save/load roundtrip.\n";

    std::cout << "All core tests passed successfully!\n";
    return 0;
}

