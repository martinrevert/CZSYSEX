# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Idioma:** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · **Português (BR)** · [Français](README.fr.md)

Aplicativo desktop nativo em **C++20** com interface acelerada por hardware (**DirectX 11 + Dear ImGui**) para o sintetizador **Casio CZ-3000** (compatível com a família Casio CZ: CZ-101, CZ-1000, CZ-3000, CZ-5000 e CZ-1).

Portado e expandido a partir do projeto Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Principais Recursos

1. **Simulação Fiel do Hardware Casio CZ-3000**:
   - Painel retrô cinza-carvão com detalhes clássicos Casio.
   - **Visor LCD 16×2 HD44780** com cursor piscante e todas as páginas de edição (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Botões Táteis com LEDs** e **Teclado Virtual de 6 Oitavas (C1–C7)** para audição MIDI em tempo real.

2. **Gráfico Interativo de Envelopes de 8 Estágios**:
   - Visualização e edição ao vivo de **DCO** (afinação), **DCW** (brilho/PD) e **DCA** (volume), individuais ou sobrepostos (`ALL`), com marcadores `SUS`/`END` e controles `Rate`/`Level` (0–99).

3. **Gerenciador de Banco Dual de 64 Vozes (32 Presets + 32 Memórias)**:
   - Abas **PRESET BANK** e **MEMORY BANK** (`*MEMORY* A-1`, com renomeação personalizada no botão direito salva em `.syx` CZ-1 de 296 bytes).
   - **Download em Lote e Cache de Presets**, além de **Auto-Fetch SysEx** ao selecionar um slot.
   - **Randomizador (`Aleatório` / `Randomize`)**: Gera valores aleatórios para todos os parâmetros do sintetizador na memória atual e envia ao vivo para o buffer de edição (`0x60`).

4. **Multi-idioma, Guia PD Interativo e Manual Oficial (8 Idiomas)**:
   - Troca instantânea entre **English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)** e **Français**.
   - **Modo Tutorial**, **Guia de Distorção de Fase** e o **Manual Oficial de Operação Casio CZ-3000** completo (9 capítulos, tabela dos 32 Presets, `INITIALIZE` e Tabela MIDI).

5. **Motor MIDI Nativo de Baixa Latência (`winmm.dll`)**:
   - Seleção de portas MIDI In/Out e SysEx oficial Casio de 263/264 bytes.

---

## 🛠️ Compilação e Execução

- **Requisitos**: Windows 10/11 (x64), Visual Studio 2022 (C++20), CMake 3.20+, Git (`--recursive`), NSIS 3.x (opcional).
- **Compilar e testar**: `build.bat` (gera `build\Release\CZ3000SysEx.exe` e executa `test_sysex.exe`).
- **Instalador**: `build_installer.bat`.

---

## 📄 Licença

Distribuído sob a licença **GNU General Public License v3.0 (GPLv3)**. Consulte [LICENSE](LICENSE).
