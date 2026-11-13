#pragma once

#include "cz_types.h"
#include <vector>
#include <string>
#include <optional>

namespace cz {

class CzSysEx {
public:
    // Encode a single patch to a SysEx message
    // If simple == true, creates 264-byte message (CZ-3000 / CZ-5000 / CZ-101)
    // If simple == false, creates 296-byte message with patch name (CZ-1)
    static std::vector<uint8_t> patch_to_sysex(const CzPatch& patch, bool simple = true, uint8_t msg_type = 0x20);

    // Decode a single SysEx message to a CzPatch
    static bool sysex_to_patch(const std::vector<uint8_t>& syx_data, CzPatch& out_patch);

    // Load multiple patches from a .syx file (can contain 1 patch or a bank of 32/64 patches)
    static bool load_syx_file(const std::string& filepath, std::vector<CzPatch>& out_patches, std::string& out_error);

    // Save patches to a .syx file (e.g. 32 patches for CZ-3000)
    static bool save_syx_file(const std::string& filepath, const std::vector<CzPatch>& patches, bool reslot = true, bool simple = true);

    // Create a request message (handshake / dump request)
    // For CZ-3000, requesting a patch dump: 0xF0 0x44 0x00 0x00 [CHAN] 0x30 [SLOT] 0xF7
    static std::vector<uint8_t> create_dump_request(uint8_t midi_channel, uint8_t slot_byte);
};

} // namespace cz

