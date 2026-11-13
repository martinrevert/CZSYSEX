#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace cz {

class CzMath {
public:
    // Nibble pack/unpack
    static uint8_t unpack_casio(uint8_t b1, uint8_t b2);
    static std::pair<uint8_t, uint8_t> pack_casio(uint8_t byte);

    // DCA calculations
    static uint8_t dca_rate_hex_to_dec(uint8_t hex_val);
    static uint8_t dca_rate_dec_to_hex(uint8_t dec_val);
    static uint8_t dca_level_hex_to_dec(uint8_t hex_val);
    static uint8_t dca_level_dec_to_hex(uint8_t dec_val);

    // DCW calculations
    static uint8_t dcw_rate_hex_to_dec(uint8_t hex_val);
    static uint8_t dcw_rate_dec_to_hex(uint8_t dec_val);
    static uint8_t dcw_level_hex_to_dec(uint8_t hex_val);
    static uint8_t dcw_level_dec_to_hex(uint8_t dec_val);

    // DCO calculations
    static uint8_t dco_rate_hex_to_dec(uint8_t hex_val);
    static uint8_t dco_rate_dec_to_hex(uint8_t dec_val);
    static uint8_t dco_level_hex_to_dec(uint8_t hex_val);
    static uint8_t dco_level_dec_to_hex(uint8_t dec_val);

    // Vibrato parameter calculation
    static std::pair<uint8_t, uint8_t> calculate_vibrato_values(int decimal, int increment = 0, int factor = 1);

    // Key follow lookup tables
    static uint8_t get_dcw_key_follow_value(int key_follow, bool simple = true);
    static uint8_t get_dca_key_follow_value(int key_follow);

    // Detune conversions
    static void detune_from_sysex(uint8_t pds, uint8_t pdetl, uint8_t pdeth, bool& negative, int& octave, int& note, int& fine);
    static void detune_to_sysex(bool negative, int octave, int note, int fine, uint8_t& pds, uint8_t& pdetl, uint8_t& pdeth);

    // Memory slot conversions
    static void memory_from_sysex(uint8_t slot_byte, bool simple, std::string& location, char& bank, int& slot);
    static uint8_t memory_to_sysex(const std::string& location, char bank, int slot, bool simple);

    // HD44780 LCD character encoding
    static char hd44780_to_ascii(uint8_t code);
    static uint8_t ascii_to_hd44780(char c);
};

} // namespace cz

