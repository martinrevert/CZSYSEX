# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Lingua:** [English](README.md) · [Español](README.es.md) · **Italiano** · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

Applicazione desktop nativa moderna in **C++20** con interfaccia grafica accelerata via hardware (**DirectX 11 + Dear ImGui**) per il sintetizzatore **Casio CZ-3000** (compatibile con la famiglia Casio CZ: CZ-101, CZ-1000, CZ-3000, CZ-5000 e CZ-1).

Porting ed espansione dal progetto Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Caratteristiche Principali

1. **Simulazione Autentica dell'Hardware Casio CZ-3000**:
   - Pannello retrò grigio antracite con dettagli classici Casio (ciano, blu scuro e rosso).
   - **Display LCD 16×2 HD44780**: Matrice di punti 5×7 verde fosforo con cursore lampeggiante e pagine di modifica reali (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Pulsanti Tattili Casio e LED di Stato**:
     - *Programmer*: `COMPARE/RECALL`, `INIT`, `WRITE`.
     - *Selezione Parametri*: `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`.
     - *Linee di Voce*: Linea 1 e Linea 2.
     - *Data Entry*: `UP`, `DOWN`, `< CUR`, `CUR >`.
     - *Banchi e Voci*: Banchi Preset e Memory `A`–`D` e Slot `1`–`8` (64 voci totali).
   - **Tastiera di Ascolto Live a 6 Ottave**: Pianoforte virtuale interattivo (**C1**–**C7**, 73 tasti) per l'ascolto MIDI in tempo reale.

2. **Grafico Interattivo degli Inviluppi a 8 Stadi**:
   - Visualizzazione e modifica in tempo reale delle curve **DCO** (intonazione, ciano), **DCW** (brillantezza/PD, ambra) e **DCA** (volume, verde lime), singolarmente o sovrapposte (`ALL`).
   - Indicatori visivi `SUS` (Sustain) ed `END` (Fine).
   - Cursori `Rate` (0–99) e `Level` (0–99) per tutti gli 8 stadi.

3. **Gestore Banco Duale a 64 Voci (32 Preset + 32 Memorie Utente)**:
   - Schede **PRESET BANK (32 Fissi)** e **MEMORY BANK (32 Utente)** con nomenclatura fedele `*MEMORY* A-1` e rinomina personalizzata con tasto destro (preservata tramite export/import `.syx` CZ-1 da 296 byte).
   - **Download Batch e Cache Preset**: Scarica tutti i 32 Preset (salvati in cache locale per non doverli rileggere ad ogni sessione) o tutte le 32 Memorie in un clic.
   - **Auto-Fetch SysEx**: Richiede automaticamente lo slot selezionato dall'hardware facendo clic su un Preset o una Memoria.
   - **Randomizzatore Patch (`Casuale` / `Randomize`)**: Genera valori casuali per tutti i parametri del synth (`Line Select`, `Octave`, `Detune`, `Vibrato`, forme d'onda, modulazione e inviluppi a 8 stadi DCO/DCW/DCA) nello slot Memory corrente e li invia dal vivo al buffer (`0x60`).
   - Invio al Buffer di Modifica (`0x60`), Scrittura su Memoria Synth, Invio Banco completo da 32 patch, Copia/Incolla tra slot e caricamento/salvataggio di file `.syx`.

4. **Interfaccia Multilingua, Guida PD Interattiva e Manuale Ufficiale (8 Lingue)**:
   - Cambio lingua istantaneo (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**) con supporto font CJK.
   - **Modalità Tutorial (`[?] MODALITÀ TUTORIAL: ON/OFF`)**: Tooltip al passaggio del mouse e finestra interattiva **Guida alla Distorsione di Fase** su DCO, DCW, DCA, inviluppi a 8 stadi, forme d'onda, modulazione Ring/Noise e SysEx.
   - **Vista Manuale Ufficiale CZ-3000**: Manuale di riferimento integrato in 9 capitoli (`Viste → Manuale Ufficiale CZ-3000`) con tabella dei 32 Preset, valori `INITIALIZE` e Tabella di Implementazione MIDI in tutte le 8 lingue.

5. **Motore MIDI Nativo a Bassa Latenza e Plugin VST3 (DAW / Ableton Live)**:
   - Basato sull'API multimediale nativa di Windows (`winmm.dll`) con selezione a caldo delle porte MIDI In/Out e trame SysEx Casio ufficiali da 263/264 byte.
   - **Plugin MIDI VST3 (`CZ3000SysEx.vst3`)**: Installabile in `C:\Program Files\Common Files\VST3` per controllare tutti i 64 patch direttamente dalle tracce MIDI delle DAW (Ableton Live, Cubase, Reaper).

---

## 🛠️ Compilazione ed Esecuzione

### Requisiti
- Windows 10 o 11 (x64)
- Visual Studio 2022 / Build Tools con supporto C++20
- CMake 3.20+
- Git con sottomoduli (`git clone --recursive` oppure `git submodule update --init --recursive`)
- *(Opzionale)* NSIS 3.x per creare il programma di installazione Windows

### Compilazione rapida e Test
```bat
build.bat
```
Genera `build\Release\CZ3000SysEx.exe`, `build\Release\CZ3000SysEx.vst3` e `build\Release\test_sysex.exe`, quindi esegue tutti i test unitari.

### Creazione Installer
```bat
build_installer.bat
```

---

## 📁 Struttura del Progetto

```
CZSYSEX/
├── docs/                  # Screenshot e risorse di documentazione
├── external/
│   └── imgui/             # Sottomodulo Dear ImGui + backend Win32/DX11
├── installer/             # Script di installazione NSIS
├── src/
│   ├── core/              # Modello dati, matematica, i18n, codec SysEx e cache banco
│   ├── midi/              # Driver MIDI Windows Multimedia (WinMM)
│   ├── ui/                # LCD, Tutorial, Manuale, Grafico Inviluppi, Librarian e Pannello
│   ├── vst3/              # Processore, vista ed entry point plugin VST3
│   └── main.cpp           # Finestra Win32, DirectX 11 e loop principale
├── tests/
│   └── test_sysex.cpp     # Test unitari per SysEx, banco e cache
├── AGENTS.md              # Linee guida per collaboratori e verifica CI
├── CMakeLists.txt
├── build.bat
└── build_installer.bat
```

---

## 📄 Licenza

Distribuito sotto la **GNU General Public License v3.0 (GPLv3)**. Vedi [LICENSE](LICENSE) per i dettagli.
