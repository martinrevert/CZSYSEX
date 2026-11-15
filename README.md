# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Language:** **English** · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

Modern native **C++20** desktop application with hardware-accelerated UI (**DirectX 11 + Dear ImGui**) for the **Casio CZ-3000** synthesizer (compatible with the Casio CZ family: CZ-101, CZ-1000, CZ-3000, CZ-5000, and CZ-1).

Ported and expanded from the Python `kasploosh` (`czsysexy` / `czcommon`) project.

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Key Features

1. **Authentic Casio CZ-3000 Hardware Simulation**:
   - Retro charcoal-gray chassis with classic Casio accents (cyan, deep blue, and red).
   - **16×2 HD44780 LCD Display**: 5×7 dot-matrix phosphor-green screen with hardware blinking cursor and real edit pages (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Tactile Casio Panel Buttons & Status LEDs**:
     - *Programmer*: `COMPARE/RECALL`, `INIT`, `WRITE`.
     - *Parameter Select*: `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`.
     - *Voice Lines*: Line 1 and Line 2.
     - *Data Entry*: `UP`, `DOWN`, `< CUR`, `CUR >`.
     - *Banks & Voices*: Preset & Memory Banks `A`–`D` and Slots `1`–`8` (64 voices total).
   - **6-Octave Live Audition Keyboard**: Interactive virtual piano (**C1** to **C7**, 73 keys) for real-time MIDI auditioning.

2. **Interactive 8-Step Envelope Graph**:
   - Real-time visualization and editing of **DCO** (pitch, cyan), **DCW** (brightness/PD, amber), and **DCA** (volume, lime green) curves, individually or layered (`ALL`).
   - Visual `SUS` (Sustain) and `END` markers.
   - `Rate` (0–99) and `Level` (0–99) sliders for all 8 steps.

3. **64-Voice Dual Bank Librarian (32 Presets + 32 User Memories)**:
   - **PRESET BANK (32 Fixed)** & **MEMORY BANK (32 User)** tabs with `*MEMORY* A-1` hardware-accurate naming and right-click custom rename (preserved via CZ-1 296-byte `.syx` export/import).
   - **Batch Download & Preset Cache**: Download all 32 Presets (cached locally so you don't need to re-fetch every session) or all 32 Memories in one click.
   - **Auto-Fetch SysEx**: Automatically requests the selected slot from the hardware when clicking a Preset or Memory slot.
   - **Synth Patch Randomizer (`Randomize`)**: Randomizes all synth parameters (`Line Select`, `Octave`, `Detune`, `Vibrato`, waveforms, modulation, and 8-stage DCO/DCW/DCA envelopes) in the current Memory slot and sends it live to the edit buffer (`0x60`).
   - Send to Edit Buffer (`0x60`), Write to Synth Memory, Send Full 32-Patch Bank, Copy/Paste between slots, and load/save `.syx` files.

4. **Multi-Language UI, Interactive PD Guide & Official Manual (8 Languages)**:
   - Full live language switching (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**) with CJK font support.
   - **Tutorial Mode (`[?] TUTORIAL MODE: ON/OFF`)**: Hover tooltips and an interactive **Phase Distortion Guide** window explaining DCO, DCW, DCA, 8-step envelopes, waveforms, ring/noise mod, and SysEx.
   - **Official CZ-3000 Operation Manual View**: Built-in 9-chapter reference manual (`Views → CZ-3000 Operation Manual`) with the 32 Preset table, `INITIALIZE` defaults, and MIDI Implementation Chart in all 8 languages.

5. **Low-Latency Native MIDI Engine**:
   - Built on native Windows Multimedia API (`winmm.dll`) with hot-swappable MIDI In/Out port selection and 263/264-byte Casio SysEx framing.

---

## 🛠️ Build & Run

### Prerequisites
- Windows 10 or 11 (x64)
- Visual Studio 2022 / Build Tools with C++20 support
- CMake 3.20+
- Git with submodules (`git clone --recursive` or `git submodule update --init --recursive`)
- *(Optional)* NSIS 3.x to package the Windows installer

### Quick Build & Test
```bat
build.bat
```
Outputs `build\Release\CZ3000SysEx.exe` and `build\Release\test_sysex.exe`, then runs all core unit tests.

### Build Installer
```bat
build_installer.bat
```

---

## 📁 Project Structure

```
CZSYSEX/
├── docs/                  # Screenshots & documentation assets
├── external/
│   └── imgui/             # Dear ImGui submodule + Win32/DX11 backends
├── installer/             # NSIS installer script
├── src/
│   ├── core/              # Data model, math, i18n, SysEx codec & bank cache
│   ├── midi/              # Windows Multimedia (WinMM) MIDI driver
│   ├── ui/                # LCD, Tutorial, Manual, Envelope Graph, Librarian & Panel
│   └── main.cpp           # Win32 window, DirectX 11 & main loop
├── tests/
│   └── test_sysex.cpp     # Core SysEx, bank & cache unit tests
├── AGENTS.md              # Contributor & CI verification guidelines
├── CMakeLists.txt
├── build.bat
└── build_installer.bat
```

---

## 📄 License

Distributed under the **GNU General Public License v3.0 (GPLv3)**. See [LICENSE](LICENSE) for details.
