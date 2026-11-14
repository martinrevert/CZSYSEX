#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <vector>

namespace cz {

enum class Model {
    CZ101,
    CZ1000,
    CZ3000,
    CZ5000,
    CZ1
};

enum class LineSelect : uint8_t {
    Line1 = 0,         // "1" (8 voices)
    Line2 = 1,         // "2" (8 voices)
    Line1_Plus_1Prime = 2, // "1+1'" (4 voices, detuned unison)
    Line1_Plus_2Prime = 3, // "1+2'" (4 voices, dual timbre)
    Line1Prime = 4,    // "1'" (muted line 1)
    Line2Prime = 5     // "2'" (muted line 1, active line 2)
};

enum class Waveform : uint8_t {
    Sawtooth   = 0, // 000
    Square     = 1, // 001
    Pulse      = 2, // 010
    DoubleSine = 3, // 011
    SawPulse   = 4, // 100
    Resonant1  = 5, // 101 (Sawtooth base)
    Resonant2  = 6, // 110 (Triangle base)
    Resonant3  = 7  // 111 (Trapezoid base)
};

enum class VibratoWave : uint8_t {
    Triangle = 1, // 0x08
    SawUp    = 2, // 0x04
    SawDown  = 3, // 0x20
    Square   = 4  // 0x02
};

enum class Modulation : uint8_t {
    Off   = 0,
    Ring  = 1,
    Noise = 2
};

struct EnvelopeStep {
    uint8_t rate  = 50; // 0 - 99
    uint8_t level = 0;  // 0 - 99
};

struct CzEnvelope {
    std::array<EnvelopeStep, 8> steps;
    int sustain_step = 1; // 1 - 8, or 0 = None ('X')
    int end_step     = 8; // 1 - 8
    int key_follow   = 0; // DCW: 0-9, DCA: 0-15
    int line_level   = 15; // DCA: 1-15 (advanced/line level)
    int velocity     = 0;  // CZ-1 velocity sensitivity

    CzEnvelope() {
        for (auto& s : steps) {
            s.rate = 50;
            s.level = 0;
        }
    }
};

struct CzLine {
    uint8_t wave_1       = 0;     // 0 - 7
    uint8_t wave_2       = 0;     // 0 - 7
    bool use_wave_2      = false; // whether wave_2 is mixed
    uint8_t window       = 0;     // 0 - 7
    uint8_t modulation   = 0;     // 0=off, 1=ring, 2=noise (Line 1 only in hardware)

    CzEnvelope dco; // Pitch envelope
    CzEnvelope dcw; // Phase distortion (Waveform/Brightness) envelope
    CzEnvelope dca; // Amplitude envelope

    CzLine();
};

struct CzDetune {
    bool negative  = false; // Sense: POS = false, NEG = true
    int octave     = 0;     // 0 - 3
    int note       = 0;     // 0 - 11
    int fine       = 0;     // 0 - 60
};

struct CzVibrato {
    int wave  = 1;  // 1: Triangle, 2: SawUp, 3: SawDown, 4: Square
    int delay = 0;  // 0 - 99
    int rate  = 0;  // 0 - 99 (Manual INITIALIZE default = 00)
    int depth = 0;  // 0 - 99
};

struct CzMemorySlot {
    std::string location = "internal"; // "internal", "preset", "cartridge"
    char bank = 'A';                   // 'A' - 'D' (or 'A'-'H' for CZ-1)
    int slot  = 1;                     // 1 - 8
};

struct CzPatch {
    std::string name = "CZ3000 SOUND";
    uint8_t midi_channel = 1; // 1 - 16
    CzMemorySlot memory;

    LineSelect line_select = LineSelect::Line1_Plus_2Prime;
    int octave = 0; // -1, 0, +1

    CzDetune detune;
    CzVibrato vibrato;

    CzLine line1;
    CzLine line2;

    bool simple_sysex = true; // True for CZ-3000/5000/1000/101 (264/265 bytes without NAME)

    CzPatch();
    static CzPatch create_default_patch(char bank = 'A', int slot = 1, const std::string& name = "INIT VOICE");
    void randomize();
};

} // namespace cz

