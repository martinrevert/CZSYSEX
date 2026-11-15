# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Lingua:** [English](README.md) · [Español](README.es.md) · **Italiano** · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

Applicazione desktop nativa **C++20** con interfaccia grafica accelerata via hardware (**DirectX 11 + Dear ImGui**) per il sintetizzatore **Casio CZ-3000** (compatibile con la famiglia Casio CZ: CZ-101, CZ-1000, CZ-3000, CZ-5000 e CZ-1).

Porting ed espansione del progetto Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Caratteristiche Principali

1. **Simulazione Fedele dell'Hardware Casio CZ-3000**:
   - Pannello grigio antracite con dettagli classici Casio (ciano, blu e rosso).
   - **Display LCD 16×2 HD44780**: Matrice 5×7 verde fosforo con cursore lampeggiante e modalità di modifica reali (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Pulsanti Tattili e LED di Stato**: `COMPARE/RECALL`, `INIT`, `WRITE`, `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`, Linee 1/2, `Data Entry` e selettore Banchi `A`–`D` / Voci `1`–`8` (64 voci).
   - **Tastiera Virtuale a 6 Ottave (C1–C7)**: 73 tasti interattivi per l'ascolto immediato via MIDI.

2. **Grafico Interattivo degli Inviluppi a 8 Stadi**:
   - Visualizzazione e modifica in tempo reale di **DCO** (intonazione), **DCW** (brillantezza/PD) e **DCA** (volume), singoli o sovrapposti (`ALL`), con indicatori `SUS` e `END` e cursori `Rate`/`Level` (0–99).

3. **Gestore Banco Duale a 64 Voci (32 Preset + 32 Memorie Utente)**:
   - Schede **PRESET BANK** e **MEMORY BANK** con formato `*MEMORY* A-1` e rinomina personalizzata con tasto destro (preservata in `.syx` CZ-1 da 296 byte).
   - **Download Batch e Cache Preset**: Scarica tutti i 32 Preset (salvati in cache locale) o le 32 Memorie in un clic.
   - **Auto-Fetch SysEx** alla selezione dello slot.
   - **Generatore Casuale (`Casuale` / `Randomize`)**: Randomizza tutti i parametri del sintetizzatore nello slot Memory corrente e li invia dal vivo al buffer (`0x60`).

4. **Interfaccia Multilingua, Guida PD e Manuale Ufficiale (8 Lingue)**:
   - Cambio lingua istantaneo (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**).
   - **Modalità Tutorial** con tooltip e **Guida alla Distorsione di Fase**.
   - **Manuale Ufficiale Casio CZ-3000** integrato (`Viste → Manuale Ufficiale CZ-3000`) con i 9 capitoli, tabella 32 Preset, valori `INITIALIZE` e tabella MIDI.

5. **Motore MIDI Nativo a Bassa Latenza (`winmm.dll`)**:
   - Selezione porte MIDI In/Out e messaggi SysEx Casio ufficiali da 263/264 byte.

---

## 🛠️ Compilazione ed Esecuzione

- **Requisiti**: Windows 10/11 (x64), Visual Studio 2022 (C++20), CMake 3.20+, Git (`--recursive`), NSIS 3.x (opzionale).
- **Compilazione e test**: `build.bat` (genera `build\Release\CZ3000SysEx.exe` ed esegue `test_sysex.exe`).
- **Installer**: `build_installer.bat`.

---

## 📄 Licenza

Distribuito sotto licenza **GNU General Public License v3.0 (GPLv3)**. Vedi [LICENSE](LICENSE).
