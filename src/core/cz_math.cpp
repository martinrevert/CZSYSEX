#include "cz_math.h"
#include <algorithm>

namespace cz {

uint8_t CzMath::unpack_casio(uint8_t b1, uint8_t b2) {
    // Casio format: b1 has low nibble, b2 has high nibble
    return ((b2 & 0x0F) << 4) | (b1 & 0x0F);
}

std::pair<uint8_t, uint8_t> CzMath::pack_casio(uint8_t byte) {
    // Casio format: low nibble first, high nibble second
    uint8_t low  = byte & 0x0F;
    uint8_t high = (byte >> 4) & 0x0F;
    return {low, high};
}

uint8_t CzMath::dca_rate_hex_to_dec(uint8_t hex_val) {
    if (hex_val == 0) return 0;
    if (hex_val >= 0x77) return 99;
    return static_cast<uint8_t>(((static_cast<int>(hex_val) * 99) / 119) + 1);
}

uint8_t CzMath::dca_rate_dec_to_hex(uint8_t dec_val) {
    if (dec_val == 0) return 0;
    if (dec_val >= 99) return 0x77;
    return static_cast<uint8_t>((0x77 * static_cast<int>(dec_val)) / 99);
}

uint8_t CzMath::dca_level_hex_to_dec(uint8_t hex_val) {
    if (hex_val == 0) return 0;
    if (hex_val < 0x1C) return 0;
    int ans = static_cast<int>(hex_val) - 0x1C;
    return static_cast<uint8_t>(std::clamp(ans, 0, 99));
}

uint8_t CzMath::dca_level_dec_to_hex(uint8_t dec_val) {
    if (dec_val == 0) return 0;
    int ans = static_cast<int>(dec_val) + 0x1C;
    return static_cast<uint8_t>(std::clamp(ans, 0, 0x7F));
}

uint8_t CzMath::dcw_rate_hex_to_dec(uint8_t hex_val) {
    if (hex_val <= 0x08) return 0;
    if (hex_val >= 0x7F) return 99;
    int ans = (((static_cast<int>(hex_val) - 8) * 99) / 119) + 1;
    return static_cast<uint8_t>(std::clamp(ans, 0, 99));
}

uint8_t CzMath::dcw_rate_dec_to_hex(uint8_t dec_val) {
    if (dec_val == 0) return 8;
    int ans = ((static_cast<int>(dec_val) * 119) / 99) + 8;
    return static_cast<uint8_t>(std::clamp(ans, 8, 0x7F));
}

uint8_t CzMath::dcw_level_hex_to_dec(uint8_t hex_val) {
    if (hex_val == 0) return 0;
    if (hex_val >= 0x7F) return 99;
    int ans = ((static_cast<int>(hex_val) * 99) / 127) + 1;
    return static_cast<uint8_t>(std::clamp(ans, 0, 99));
}

uint8_t CzMath::dcw_level_dec_to_hex(uint8_t dec_val) {
    if (dec_val == 0) return 0;
    int ans = (static_cast<int>(dec_val) * 127) / 99;
    return static_cast<uint8_t>(std::clamp(ans, 0, 0x7F));
}

uint8_t CzMath::dco_rate_hex_to_dec(uint8_t hex_val) {
    if (hex_val == 0) return 0;
    if (hex_val >= 0x7F) return 99;
    int ans = ((static_cast<int>(hex_val) * 99) / 127) + 1;
    return static_cast<uint8_t>(std::clamp(ans, 0, 99));
}

uint8_t CzMath::dco_rate_dec_to_hex(uint8_t dec_val) {
    if (dec_val == 0) return 0;
    int ans = (static_cast<int>(dec_val) * 127) / 99;
    return static_cast<uint8_t>(std::clamp(ans, 0, 0x7F));
}

uint8_t CzMath::dco_level_hex_to_dec(uint8_t hex_val) {
    if (hex_val >= 0x44) {
        return hex_val - 0x04;
    }
    return hex_val;
}

uint8_t CzMath::dco_level_dec_to_hex(uint8_t dec_val) {
    if (dec_val < 64) {
        return dec_val;
    }
    return dec_val + 0x04;
}

std::pair<uint8_t, uint8_t> CzMath::calculate_vibrato_values(int decimal, int increment, int factor) {
    int offset = (decimal / 16) - 1;
    decimal += increment;

    int total = 0;
    if (offset <= 0) {
        total = decimal;
    } else {
        int base = decimal - (16 * offset);
        total = (base << offset) | ((1 << offset) - 1);
    }

    total *= factor;
    total = std::clamp(total, 0, 0xFFFF);

    return {static_cast<uint8_t>(total % 256), static_cast<uint8_t>(total / 256)};
}

uint8_t CzMath::get_dcw_key_follow_value(int key_follow, bool simple) {
    static const uint8_t simple_vals[10] = {
        0x00, 0x1F, 0x2C, 0x39, 0x46, 0x53, 0x60, 0x6E, 0x92, 0xFF
    };
    static const uint8_t advanced_vals[10] = {
        0x00, 0x19, 0x33, 0x4E, 0x6A, 0x86, 0xA3, 0xC1, 0xDF, 0xFF
    };
    int k = std::clamp(key_follow, 0, 9);
    return simple ? simple_vals[k] : advanced_vals[k];
}

uint8_t CzMath::get_dca_key_follow_value(int key_follow) {
    static const uint8_t vals[16] = {
        0x00, 0x08, 0x11, 0x1A, 0x24, 0x2F, 0x3A, 0x45,
        0x52, 0x5F, 0x6D, 0x7C, 0x8C, 0x9D, 0xAF, 0xC2
    };
    int k = std::clamp(key_follow, 0, 15);
    return vals[k];
}

void CzMath::detune_from_sysex(uint8_t pds, uint8_t pdetl, uint8_t pdeth, bool& negative, int& octave, int& note, int& fine) {
    negative = (pds == 0x01);
    pdetl /= 4;
    int subtrahend = pdetl / 16;
    fine = pdetl - subtrahend;
    octave = pdeth / 12;
    note = pdeth % 12;
}

void CzMath::detune_to_sysex(bool negative, int octave, int note, int fine, uint8_t& pds, uint8_t& pdetl, uint8_t& pdeth) {
    pds = negative ? 0x01 : 0x00;
    int addend = (fine - 1) / 15;
    int val = fine + addend;
    if (val < 0) val = 0;
    val *= 4;
    pdetl = static_cast<uint8_t>(val);
    pdeth = static_cast<uint8_t>((octave * 12) + note);
}

void CzMath::memory_from_sysex(uint8_t slot_byte, bool simple, std::string& location, char& bank, int& slot) {
    if (slot_byte == 0x60) {
        location = "recall";
        bank = 'A';
        slot = 1;
        return;
    }
    location = "internal";
    if (simple && slot_byte >= 0x20) {
        slot_byte -= 0x20;
    }
    int bank_index = (slot_byte >> 3) & 0x07;
    int slot_index = slot_byte & 0x07;
    bank = static_cast<char>('A' + bank_index);
    slot = slot_index + 1;
}

uint8_t CzMath::memory_to_sysex(const std::string& location, char bank, int slot, bool simple) {
    if (location == "recall") {
        return 0x60;
    }
    int bank_index = std::clamp(bank - 'A', 0, 7);
    int slot_index = std::clamp(slot - 1, 0, 7);
    uint8_t res = static_cast<uint8_t>((bank_index << 3) | slot_index);
    if (simple) {
        res += 0x20;
    }
    return res;
}

char CzMath::hd44780_to_ascii(uint8_t code) {
    if (code >= 32 && code <= 126) {
        return static_cast<char>(code);
    }
    return ' ';
}

uint8_t CzMath::ascii_to_hd44780(char c) {
    uint8_t uc = static_cast<uint8_t>(c);
    if (uc >= 32 && uc <= 126) {
        return uc;
    }
    return 32; // Space
}

} // namespace cz

