#include "cz_sysex.h"
#include "cz_math.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace cz {

namespace {

void encode_envelope(const CzEnvelope& env, const std::string& type, std::vector<uint8_t>& out_bytes) {
    // 8 steps -> 16 bytes (rate, level interleaved)
    for (int i = 0; i < 8; ++i) {
        uint8_t r_dec = env.steps[i].rate;
        uint8_t l_dec = env.steps[i].level;

        uint8_t r_hex = 0;
        uint8_t l_hex = 0;

        if (type == "dca") {
            r_hex = CzMath::dca_rate_dec_to_hex(r_dec);
            l_hex = CzMath::dca_level_dec_to_hex(l_dec);
        } else if (type == "dcw") {
            r_hex = CzMath::dcw_rate_dec_to_hex(r_dec);
            l_hex = CzMath::dcw_level_dec_to_hex(l_dec);
        } else { // dco
            r_hex = CzMath::dco_rate_dec_to_hex(r_dec);
            l_hex = CzMath::dco_level_dec_to_hex(l_dec);
        }

        // Downward slope bit on rate
        if ((i == 0 && l_dec == 0) || (i > 0 && l_dec < env.steps[i - 1].level)) {
            r_hex |= 0x80;
        }

        // Sustain step bit on level
        if (env.sustain_step == (i + 1)) {
            l_hex |= 0x80;
        }

        out_bytes.push_back(r_hex);
        out_bytes.push_back(l_hex);
    }
}

void decode_envelope(const uint8_t* pxx, const std::string& type, CzEnvelope& env) {
    env.sustain_step = 0; // default none
    int max_step = (env.end_step >= 1 && env.end_step <= 8) ? env.end_step : 8;
    for (int i = 0; i < 8; ++i) {
        uint8_t r_byte = pxx[i * 2];
        uint8_t l_byte = pxx[i * 2 + 1];

        if ((l_byte & 0x80) && (i + 1) <= max_step) {
            env.sustain_step = i + 1;
        }

        uint8_t r_clean = r_byte & 0x7F;
        uint8_t l_clean = l_byte & 0x7F;

        if (type == "dca") {
            env.steps[i].rate = CzMath::dca_rate_hex_to_dec(r_clean);
            env.steps[i].level = CzMath::dca_level_hex_to_dec(l_clean);
        } else if (type == "dcw") {
            env.steps[i].rate = CzMath::dcw_rate_hex_to_dec(r_clean);
            env.steps[i].level = CzMath::dcw_level_hex_to_dec(l_clean);
        } else { // dco
            env.steps[i].rate = CzMath::dco_rate_hex_to_dec(r_clean);
            env.steps[i].level = CzMath::dco_level_hex_to_dec(l_clean);
        }
    }
}

} // anonymous namespace

std::vector<uint8_t> CzSysEx::patch_to_sysex(const CzPatch& patch, bool simple, uint8_t msg_type) {
    std::vector<uint8_t> unpacked;

    // Header bytes (not nibble-packed)
    uint8_t chan_byte = static_cast<uint8_t>(patch.midi_channel + 0x6F);
    uint8_t slot_byte = CzMath::memory_to_sysex(patch.memory.location, patch.memory.bank, patch.memory.slot, simple);

    unpacked.push_back(chan_byte);
    unpacked.push_back(msg_type);
    unpacked.push_back(slot_byte);

    // From here, data is packed into nibbles
    // PFLAG: octave (bits 4-5) and line_select (bits 6-7)
    uint8_t oct_bits = 0;
    if (patch.octave == 1) oct_bits = 1;      // +1 -> 01
    else if (patch.octave == -1) oct_bits = 2; // -1 -> 10
    else oct_bits = 0;                        //  0 -> 00

    uint8_t line_bits = 0;
    switch (patch.line_select) {
        case LineSelect::Line1: line_bits = 0; break;
        case LineSelect::Line2: line_bits = 1; break;
        case LineSelect::Line1_Plus_1Prime:
        case LineSelect::Line1Prime: line_bits = 2; break;
        case LineSelect::Line1_Plus_2Prime:
        case LineSelect::Line2Prime:
        default: line_bits = 3; break;
    }

    uint8_t pflag = (oct_bits << 2) | line_bits;
    unpacked.push_back(pflag);

    // Detune: PDS, PDETL, PDETH
    uint8_t pds = 0, pdetl = 0, pdeth = 0;
    CzMath::detune_to_sysex(patch.detune.negative, patch.detune.octave, patch.detune.note, patch.detune.fine, pds, pdetl, pdeth);
    unpacked.push_back(pds);
    unpacked.push_back(pdetl);
    unpacked.push_back(pdeth);

    // Vibrato: PVK, PVDLD, PVDLV (2), PVSD, PVSV (2), PVDD, PVDV (2)
    uint8_t pvk = 0x08; // Triangle
    if (patch.vibrato.wave == 2) pvk = 0x04; // SawUp
    else if (patch.vibrato.wave == 3) pvk = 0x20; // SawDown
    else if (patch.vibrato.wave == 4) pvk = 0x02; // Square

    auto pvdlv = CzMath::calculate_vibrato_values(patch.vibrato.delay, 0, 1);
    auto pvsv  = CzMath::calculate_vibrato_values(patch.vibrato.rate, 1, 0x20);
    auto pvdv  = CzMath::calculate_vibrato_values(patch.vibrato.depth, 1, 1);

    unpacked.push_back(pvk);
    unpacked.push_back(static_cast<uint8_t>(patch.vibrato.delay));
    unpacked.push_back(pvdlv.first);
    unpacked.push_back(pvdlv.second);
    unpacked.push_back(static_cast<uint8_t>(patch.vibrato.rate));
    unpacked.push_back(pvsv.first);
    unpacked.push_back(pvsv.second);
    unpacked.push_back(static_cast<uint8_t>(patch.vibrato.depth));
    unpacked.push_back(pvdv.first);
    unpacked.push_back(pvdv.second);

    // Line 1: MFW (2 bytes)
    // Hardware MFW modulation bits [5:3]: 0 = OFF, 4 (0b100, 0x20) = RING, 3 (0b011, 0x18) = NOISE
    bool mute_line_1 = (patch.line_select == LineSelect::Line1Prime || patch.line_select == LineSelect::Line2Prime);
    uint16_t hw_mod = 0;
    if (patch.line1.modulation == 1 || patch.line1.modulation == 4) {
        hw_mod = 4; // Ring
    } else if (patch.line1.modulation == 2 || patch.line1.modulation == 3 || patch.line1.modulation == 7) {
        hw_mod = 3; // Noise
    }
    uint16_t mfw = 0;
    mfw |= (static_cast<uint16_t>(patch.line1.wave_1 & 0x07) << 13);
    mfw |= (static_cast<uint16_t>(patch.line1.wave_2 & 0x07) << 10);
    if (patch.line1.use_wave_2) mfw |= (1 << 9);
    mfw |= (static_cast<uint16_t>(patch.line1.window & 0x07) << 6);
    mfw |= (hw_mod << 3);
    if (mute_line_1) mfw |= (1 << 2);

    unpacked.push_back(static_cast<uint8_t>(mfw >> 8));
    unpacked.push_back(static_cast<uint8_t>(mfw & 0xFF));

    // Line 1 DCA key follow & level: MAMD, MAMV
    uint8_t mamd = static_cast<uint8_t>(patch.line1.dca.key_follow);
    if (!simple) {
        int lvl = std::clamp(patch.line1.dca.line_level, 1, 15);
        mamd = static_cast<uint8_t>((std::clamp((lvl * -1) + 15, 0, 0xE) * 0x10) + patch.line1.dca.key_follow);
    }
    unpacked.push_back(mamd);
    unpacked.push_back(CzMath::get_dca_key_follow_value(patch.line1.dca.key_follow));

    // Line 1 DCW key follow: MWMD, MWMV
    unpacked.push_back(static_cast<uint8_t>(patch.line1.dcw.key_follow));
    unpacked.push_back(CzMath::get_dcw_key_follow_value(patch.line1.dcw.key_follow, simple));

    // Line 1 DCA envelope: PMAL, PMA (16)
    uint8_t pmal = static_cast<uint8_t>(patch.line1.dca.end_step - 1);
    if (!simple) pmal += static_cast<uint8_t>(patch.line1.dca.velocity * 0x10);
    unpacked.push_back(pmal);
    encode_envelope(patch.line1.dca, "dca", unpacked);

    // Line 1 DCW envelope: PMWL, PMW (16)
    uint8_t pmwl = static_cast<uint8_t>(patch.line1.dcw.end_step - 1);
    if (!simple) pmwl += static_cast<uint8_t>(patch.line1.dcw.velocity * 0x10);
    unpacked.push_back(pmwl);
    encode_envelope(patch.line1.dcw, "dcw", unpacked);

    // Line 1 DCO envelope: PMPL, PMP (16)
    uint8_t pmpl = static_cast<uint8_t>(patch.line1.dco.end_step - 1);
    if (!simple) pmpl += static_cast<uint8_t>(patch.line1.dco.velocity * 0x10);
    unpacked.push_back(pmpl);
    encode_envelope(patch.line1.dco, "dco", unpacked);

    // Line 2: SFW (2 bytes)
    uint16_t sfw = 0;
    sfw |= (static_cast<uint16_t>(patch.line2.wave_1 & 0x07) << 13);
    sfw |= (static_cast<uint16_t>(patch.line2.wave_2 & 0x07) << 10);
    if (patch.line2.use_wave_2) sfw |= (1 << 9);
    sfw |= (static_cast<uint16_t>(patch.line2.window & 0x07) << 6);
    // Line 2 has modulation = 000
    unpacked.push_back(static_cast<uint8_t>(sfw >> 8));
    unpacked.push_back(static_cast<uint8_t>(sfw & 0xFF));

    // Line 2 DCA key follow: SAMD, SAMV
    uint8_t samd = static_cast<uint8_t>(patch.line2.dca.key_follow);
    if (!simple) {
        int lvl = std::clamp(patch.line2.dca.line_level, 1, 15);
        samd = static_cast<uint8_t>((std::clamp((lvl * -1) + 15, 0, 0xE) * 0x10) + patch.line2.dca.key_follow);
    }
    unpacked.push_back(samd);
    unpacked.push_back(CzMath::get_dca_key_follow_value(patch.line2.dca.key_follow));

    // Line 2 DCW key follow: SWMD, SWMV
    unpacked.push_back(static_cast<uint8_t>(patch.line2.dcw.key_follow));
    unpacked.push_back(CzMath::get_dcw_key_follow_value(patch.line2.dcw.key_follow, simple));

    // Line 2 DCA envelope: PSAL, PSA (16)
    uint8_t psal = static_cast<uint8_t>(patch.line2.dca.end_step - 1);
    if (!simple) psal += static_cast<uint8_t>(patch.line2.dca.velocity * 0x10);
    unpacked.push_back(psal);
    encode_envelope(patch.line2.dca, "dca", unpacked);

    // Line 2 DCW envelope: PSWL, PSW (16)
    uint8_t pswl = static_cast<uint8_t>(patch.line2.dcw.end_step - 1);
    if (!simple) pswl += static_cast<uint8_t>(patch.line2.dcw.velocity * 0x10);
    unpacked.push_back(pswl);
    encode_envelope(patch.line2.dcw, "dcw", unpacked);

    // Line 2 DCO envelope: PSPL, PSP (16)
    uint8_t pspl = static_cast<uint8_t>(patch.line2.dco.end_step - 1);
    if (!simple) pspl += static_cast<uint8_t>(patch.line2.dco.velocity * 0x10);
    unpacked.push_back(pspl);
    encode_envelope(patch.line2.dco, "dco", unpacked);

    // Optional patch name for CZ-1 advanced mode
    if (!simple) {
        std::string n = patch.name;
        if (n.size() < 16) n.resize(16, ' ');
        for (int i = 0; i < 16; ++i) {
            unpacked.push_back(CzMath::ascii_to_hd44780(n[i]));
        }
    }

    // Pack into final sysex
    std::vector<uint8_t> packed;
    packed.push_back(0xF0);
    packed.push_back(0x44); // Casio ID
    packed.push_back(0x00);
    packed.push_back(0x00);

    // First 3 bytes unpacked
    packed.push_back(unpacked[0]); // CHAN
    packed.push_back(unpacked[1]); // MSG
    packed.push_back(unpacked[2]); // SLOT

    // Pack remaining into nibbles
    for (size_t i = 3; i < unpacked.size(); ++i) {
        auto nibbles = CzMath::pack_casio(unpacked[i]);
        packed.push_back(nibbles.first);
        packed.push_back(nibbles.second);
    }

    packed.push_back(0xF7); // End of SysEx
    return packed;
}

bool CzSysEx::sysex_to_patch(const std::vector<uint8_t>& syx_data, CzPatch& out_patch) {
    if (syx_data.size() < 263 || syx_data[0] != 0xF0 || syx_data[1] != 0x44 || syx_data.back() != 0xF7) {
        return false;
    }

    std::vector<uint8_t> data = syx_data;
    // Missing memory slot byte correction (if 263 or 295 bytes)
    if (data.size() == 263 || data.size() == 295) {
        data.insert(data.begin() + 6, 0x60); // Recall / edit buffer
    }

    bool simple = (data.size() == 264);
    out_patch.simple_sysex = simple;

    // Unpack
    std::vector<uint8_t> unpacked;
    unpacked.push_back(data[4]); // CHAN
    unpacked.push_back(data[5]); // MSG
    unpacked.push_back(data[6]); // SLOT

    for (size_t i = 7; i + 1 < data.size() - 1; i += 2) {
        unpacked.push_back(CzMath::unpack_casio(data[i], data[i + 1]));
    }

    if (unpacked.size() < 127) {
        return false;
    }

    // CHAN
    out_patch.midi_channel = static_cast<uint8_t>(std::clamp(unpacked[0] - 0x6F, 1, 16));

    // Memory slot
    CzMath::memory_from_sysex(unpacked[2], simple, out_patch.memory.location, out_patch.memory.bank, out_patch.memory.slot);

    // PFLAG: Octave and line select
    uint8_t pflag = unpacked[3];
    uint8_t oct_bits = (pflag >> 2) & 0x03;
    if (oct_bits == 1) out_patch.octave = 1;
    else if (oct_bits == 2) out_patch.octave = -1;
    else out_patch.octave = 0;

    uint8_t line_bits = pflag & 0x03;
    switch (line_bits) {
        case 0: out_patch.line_select = LineSelect::Line1; break;
        case 1: out_patch.line_select = LineSelect::Line2; break;
        case 2: out_patch.line_select = LineSelect::Line1_Plus_1Prime; break;
        case 3:
        default: out_patch.line_select = LineSelect::Line1_Plus_2Prime; break;
    }

    // Detune
    CzMath::detune_from_sysex(unpacked[4], unpacked[5], unpacked[6],
                              out_patch.detune.negative, out_patch.detune.octave,
                              out_patch.detune.note, out_patch.detune.fine);

    // Vibrato
    uint8_t pvk = unpacked[7];
    if (pvk == 0x04) out_patch.vibrato.wave = 2; // SawUp
    else if (pvk == 0x20) out_patch.vibrato.wave = 3; // SawDown
    else if (pvk == 0x02) out_patch.vibrato.wave = 4; // Square
    else out_patch.vibrato.wave = 1; // Triangle

    out_patch.vibrato.delay = unpacked[8];
    out_patch.vibrato.rate  = unpacked[11];
    out_patch.vibrato.depth = unpacked[14];

    // Line 1: MFW
    uint16_t mfw = (static_cast<uint16_t>(unpacked[17]) << 8) | unpacked[18];
    out_patch.line1.wave_1 = (mfw >> 13) & 0x07;
    out_patch.line1.wave_2 = (mfw >> 10) & 0x07;
    out_patch.line1.use_wave_2 = ((mfw >> 9) & 0x01) != 0;
    out_patch.line1.window = (mfw >> 6) & 0x07;
    int raw_mod = (mfw >> 3) & 0x07;
    if (raw_mod == 4 || raw_mod == 1) {
        out_patch.line1.modulation = 1; // Ring
    } else if (raw_mod == 3 || raw_mod == 7 || raw_mod == 2) {
        out_patch.line1.modulation = 2; // Noise
    } else {
        out_patch.line1.modulation = 0; // Off
    }

    bool mute_line_1 = ((mfw >> 2) & 0x01) != 0;
    if (mute_line_1) {
        if (out_patch.line_select == LineSelect::Line1_Plus_1Prime) out_patch.line_select = LineSelect::Line1Prime;
        else if (out_patch.line_select == LineSelect::Line1_Plus_2Prime) out_patch.line_select = LineSelect::Line2Prime;
    }

    // Line 1 DCA key follow
    uint8_t mamd = unpacked[19];
    out_patch.line1.dca.key_follow = mamd & 0x0F;
    out_patch.line1.dca.line_level = simple ? 15 : std::clamp(15 - (mamd >> 4), 1, 15);

    // Line 1 DCW key follow
    out_patch.line1.dcw.key_follow = unpacked[21] & 0x0F;

    // Line 1 DCA envelope
    uint8_t pmal = unpacked[23];
    out_patch.line1.dca.end_step = (pmal & 0x0F) + 1;
    out_patch.line1.dca.velocity = pmal >> 4;
    decode_envelope(&unpacked[24], "dca", out_patch.line1.dca);

    // Line 1 DCW envelope
    uint8_t pmwl = unpacked[40];
    out_patch.line1.dcw.end_step = (pmwl & 0x0F) + 1;
    out_patch.line1.dcw.velocity = pmwl >> 4;
    decode_envelope(&unpacked[41], "dcw", out_patch.line1.dcw);

    // Line 1 DCO envelope
    uint8_t pmpl = unpacked[57];
    out_patch.line1.dco.end_step = (pmpl & 0x0F) + 1;
    out_patch.line1.dco.velocity = pmpl >> 4;
    decode_envelope(&unpacked[58], "dco", out_patch.line1.dco);

    // Line 2: SFW
    uint16_t sfw = (static_cast<uint16_t>(unpacked[74]) << 8) | unpacked[75];
    out_patch.line2.wave_1 = (sfw >> 13) & 0x07;
    out_patch.line2.wave_2 = (sfw >> 10) & 0x07;
    out_patch.line2.use_wave_2 = ((sfw >> 9) & 0x01) != 0;
    out_patch.line2.window = (sfw >> 6) & 0x07;
    out_patch.line2.modulation = 0;

    // Line 2 DCA key follow
    uint8_t samd = unpacked[76];
    out_patch.line2.dca.key_follow = samd & 0x0F;
    out_patch.line2.dca.line_level = simple ? 15 : std::clamp(15 - (samd >> 4), 1, 15);

    // Line 2 DCW key follow
    out_patch.line2.dcw.key_follow = unpacked[78] & 0x0F;

    // Line 2 DCA envelope
    uint8_t psal = unpacked[80];
    out_patch.line2.dca.end_step = (psal & 0x0F) + 1;
    out_patch.line2.dca.velocity = psal >> 4;
    decode_envelope(&unpacked[81], "dca", out_patch.line2.dca);

    // Line 2 DCW envelope
    uint8_t pswl = unpacked[97];
    out_patch.line2.dcw.end_step = (pswl & 0x0F) + 1;
    out_patch.line2.dcw.velocity = pswl >> 4;
    decode_envelope(&unpacked[98], "dcw", out_patch.line2.dcw);

    // Line 2 DCO envelope
    uint8_t pspl = unpacked[114];
    out_patch.line2.dco.end_step = (pspl & 0x0F) + 1;
    out_patch.line2.dco.velocity = pspl >> 4;
    decode_envelope(&unpacked[115], "dco", out_patch.line2.dco);

    // Detect all-zero / completely silent empty SysEx slots (common in partial 16-slot CZ-101 dumps like ANIMAL.SYX)
    bool all_zero_payload = true;
    for (size_t i = 3; i < (std::min)(unpacked.size(), static_cast<size_t>(131)); ++i) {
        if (unpacked[i] != 0) {
            all_zero_payload = false;
            break;
        }
    }
    int max_l1_dca = 0;
    for (int i = 0; i < std::clamp(out_patch.line1.dca.end_step, 1, 8); ++i) {
        max_l1_dca = (std::max)(max_l1_dca, static_cast<int>(out_patch.line1.dca.steps[i].level));
    }
    int max_l2_dca = 0;
    for (int i = 0; i < std::clamp(out_patch.line2.dca.end_step, 1, 8); ++i) {
        max_l2_dca = (std::max)(max_l2_dca, static_cast<int>(out_patch.line2.dca.steps[i].level));
    }
    bool uses_l1 = (out_patch.line_select == LineSelect::Line1 ||
                    out_patch.line_select == LineSelect::Line1_Plus_1Prime ||
                    out_patch.line_select == LineSelect::Line1_Plus_2Prime ||
                    out_patch.line_select == LineSelect::Line1Prime);
    bool uses_l2 = (out_patch.line_select == LineSelect::Line2 ||
                    out_patch.line_select == LineSelect::Line1_Plus_2Prime ||
                    out_patch.line_select == LineSelect::Line2Prime);
    bool silent_dca = ((uses_l1 ? max_l1_dca : 0) == 0) && ((uses_l2 ? max_l2_dca : 0) == 0);
    bool is_empty_slot = all_zero_payload || silent_dca;

    // Name (if advanced / 296 bytes)
    if (!simple && unpacked.size() >= 147) {
        std::string name_str;
        for (int i = 0; i < 16; ++i) {
            name_str += CzMath::hd44780_to_ascii(unpacked[131 + i]);
        }
        // Trim trailing spaces
        size_t last = name_str.find_last_not_of(' ');
        if (last != std::string::npos) {
            name_str = name_str.substr(0, last + 1);
        }
        out_patch.name = is_empty_slot ? ("[EMPTY] " + name_str) : name_str;
    } else if (is_empty_slot) {
        out_patch.name = "[EMPTY]";
    } else {
        std::stringstream ss;
        ss << out_patch.memory.bank << out_patch.memory.slot << " PATCH";
        out_patch.name = ss.str();
    }

    return true;
}

bool CzSysEx::load_syx_file(const std::string& filepath, std::vector<CzPatch>& out_patches, std::string& out_error) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        out_error = "Could not open file: " + filepath;
        return false;
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (buffer.empty()) {
        out_error = "File is empty.";
        return false;
    }

    // Split buffer by 0xF0 ... 0xF7
    size_t pos = 0;
    while (pos < buffer.size()) {
        if (buffer[pos] == 0xF0) {
            size_t end_pos = pos + 1;
            while (end_pos < buffer.size() && buffer[end_pos] != 0xF7) {
                if (buffer[end_pos] == 0xF0) break; // Unexpected new F0
                end_pos++;
            }
            if (end_pos < buffer.size() && buffer[end_pos] == 0xF7) {
                std::vector<uint8_t> msg(buffer.begin() + pos, buffer.begin() + end_pos + 1);
                CzPatch patch;
                if (sysex_to_patch(msg, patch)) {
                    out_patches.push_back(patch);
                }
                pos = end_pos + 1;
                continue;
            }
        }
        pos++;
    }

    if (out_patches.empty()) {
        out_error = "No valid CZ SysEx timbre messages found in file.";
        return false;
    }

    return true;
}

bool CzSysEx::save_syx_file(const std::string& filepath, const std::vector<CzPatch>& patches, bool reslot, bool simple) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    static const char banks[] = {'A', 'B', 'C', 'D'};
    for (size_t i = 0; i < patches.size(); ++i) {
        CzPatch p = patches[i];
        if (reslot && i < 32) {
            p.memory.location = "internal";
            p.memory.bank = banks[i / 8];
            p.memory.slot = static_cast<int>((i % 8) + 1);
        }
        auto syx = patch_to_sysex(p, simple);
        file.write(reinterpret_cast<const char*>(syx.data()), syx.size());
    }

    return true;
}

std::vector<uint8_t> CzSysEx::create_dump_request(uint8_t midi_channel, uint8_t slot_byte) {
    uint8_t chan_byte = static_cast<uint8_t>(std::clamp(static_cast<int>(midi_channel), 1, 16) + 0x6F);
    return {
        0xF0, 0x44, 0x00, 0x00,
        chan_byte,
        0x30, // Request / Handshake ask
        slot_byte,
        0xF7
    };
}

} // namespace cz

