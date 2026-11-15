# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Sprache:** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · **Deutsch** · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

Moderne native **C++20**-Desktop-Anwendung mit hardwarebeschleunigter Oberfläche (**DirectX 11 + Dear ImGui**) für den **Casio CZ-3000** Synthesizer (kompatibel mit CZ-101, CZ-1000, CZ-3000, CZ-5000 und CZ-1).

Portiert und erweitert aus dem Python-Projekt `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Hauptfunktionen

1. **Authentische Casio CZ-3000 Hardware-Simulation**:
   - Retro-Gehäuse in Anthrazit mit klassischen Casio-Akzenten.
   - **16×2 HD44780 LCD-Display** mit blinkendem Cursor und allen Edit-Seiten (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Hardware-Taster mit Status-LEDs** sowie **6-Oktaven-Tastatur (C1–C7)** für Echtzeit-MIDI-Vorhören.

2. **Interaktive 8-Stufen-Hüllkurvengrafik**:
   - Echtzeit-Ansicht und -Bearbeitung für **DCO** (Tonhöhe), **DCW** (Helligkeit/PD) und **DCA** (Lautstärke), einzeln oder überlagert (`ALL`), inklusive `SUS`- und `END`-Markierungen sowie `Rate`/`Level`-Reglern (0–99).

3. **64-Stimmen Dual-Bank-Librarian (32 Presets + 32 User-Memories)**:
   - **PRESET BANK** & **MEMORY BANK** (`*MEMORY* A-1`) mit Rechtsklick-Umbenennung (gespeichert im 296-Byte CZ-1 `.syx`-Format).
   - **Batch-Download & Preset-Cache** sowie **Auto-Fetch SysEx** bei Klick auf einen Speicherplatz.
   - **Zufallsgenerator (`Zufall` / `Randomize`)**: Erzeugt zufällige Synthesizer-Parameter im aktuellen Memory-Slot und sendet sie direkt an den Edit-Buffer (`0x60`).

4. **Mehrsprachige Oberfläche, PD-Handbuch & Offizielle Anleitung (8 Sprachen)**:
   - Sofortiger Sprachwechsel (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**).
   - **Tutorial-Modus** mit Tooltips, **Phase-Distortion-Handbuch** und der vollständigen **Casio CZ-3000 Bedienungsanleitung** (9 Kapitel + 32-Preset-Tabelle + `INITIALIZE` + MIDI-Implementation).

5. **Latenzarme native MIDI-Engine (`winmm.dll`)**:
   - Hot-Swap für MIDI In/Out und offizielle 263/264-Byte Casio-SysEx-Übertragung.

---

## 🛠️ Kompilieren & Ausführen

- **Voraussetzungen**: Windows 10/11 (x64), Visual Studio 2022 (C++20), CMake 3.20+, Git (`--recursive`), NSIS 3.x (optional).
- **Build & Tests**: `build.bat` (erzeugt `build\Release\CZ3000SysEx.exe` und startet `test_sysex.exe`).
- **Installer**: `build_installer.bat`.

---

## 📄 Lizenz

Veröffentlicht unter der **GNU General Public License v3.0 (GPLv3)**. Details siehe [LICENSE](LICENSE).
