# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Idioma:** [English](README.md) · [Español](README.es.md) · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · **Português (BR)** · [Français](README.fr.md)

Aplicativo desktop nativo moderno em **C++20** com interface gráfica acelerada por hardware (**DirectX 11 + Dear ImGui**) para o sintetizador **Casio CZ-3000** (compatível com toda a família Casio CZ: CZ-101, CZ-1000, CZ-3000, CZ-5000 e CZ-1).

Portado e expandido a partir do projeto Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Principais Recursos

1. **Simulação Autêntica do Hardware Casio CZ-3000**:
   - Chassi retrô cinza-carvão com detalhes clássicos Casio (ciano, azul escuro e vermelho).
   - **Visor LCD 16×2 HD44780**: Matriz de pontos 5×7 verde fósforo com cursor piscante e páginas reais de edição (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Botões Táteis Casio e LEDs de Status**:
     - *Programmer*: `COMPARE/RECALL`, `INIT`, `WRITE`.
     - *Seleção de Parâmetro*: `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`.
     - *Linhas de Voz*: Linha 1 e Linha 2.
     - *Data Entry*: `UP`, `DOWN`, `< CUR`, `CUR >`.
     - *Bancos e Vozes*: Bancos Preset e Memory `A`–`D` e Slots `1`–`8` (64 vozes no total).
   - **Teclado de Audição ao Vivo de 6 Oitavas**: Piano virtual interativo (**C1** a **C7**, 73 teclas) para audição MIDI em tempo real.

2. **Gráfico Interativo de Envelopes de 8 Estágios**:
   - Visualização e edição em tempo real das curvas **DCO** (afinação, ciano), **DCW** (brilho/PD, âmbar) e **DCA** (volume, verde-limão), individualmente ou sobrepostas (`ALL`).
   - Marcadores visuais de `SUS` (Sustain) e `END` (Fim).
   - Controles deslizantes de `Rate` (0–99) e `Level` (0–99) para todos os 8 estágios.

3. **Gerenciador de Banco Dual de 64 Vozes (32 Presets + 32 Memórias de Usuário)**:
   - Abas **PRESET BANK (32 Fixos)** e **MEMORY BANK (32 Usuário)** com nomenclatura fiel `*MEMORY* A-1` e renomeação personalizada com o botão direito (preservada via exportação/importação `.syx` CZ-1 de 296 bytes).
   - **Download em Lote e Cache de Presets**: Baixe todos os 32 Presets (armazenados em cache local para não precisar reler a cada sessão) ou todas as 32 Memórias em um clique.
   - **Auto-Fetch SysEx**: Solicita automaticamente o slot selecionado ao hardware ao clicar em um Preset ou Memória.
   - **Randomizador de Patches (`Aleatório` / `Randomize`)**: Randomiza todos os parâmetros do sintetizador (`Line Select`, `Octave`, `Detune`, `Vibrato`, formas de onda, modulação e envelopes de 8 estágios DCO/DCW/DCA) na memória atual e envia ao vivo para o buffer de edição (`0x60`).
   - Enviar ao Buffer de Edição (`0x60`), Gravar na Memória do Synth, Enviar Banco Completo de 32 patches, Copiar/Colar entre slots e abrir/salvar arquivos `.syx`.

4. **Interface Multi-idioma, Guia PD Interativo e Manual Oficial (8 Idiomas)**:
   - Troca instantânea de idioma (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**) com suporte a fontes CJK.
   - **Modo Tutorial (`[?] MODO TUTORIAL: ON/OFF`)**: Tooltips ao passar o mouse e janela interativa **Guia de Distorção de Fase** explicando DCO, DCW, DCA, envelopes de 8 estágios, ondas, modulação Ring/Noise e SysEx.
   - **Visualização do Manual Oficial CZ-3000**: Manual de referência integrado com 9 capítulos (`Visualizações → Manual Oficial CZ-3000`), tabela dos 32 Presets, padrões `INITIALIZE` e Tabela de Implementação MIDI nos 8 idiomas.

5. **Motor MIDI Nativo de Baixa Latência**:
   - Construído sobre a API multimídia nativa do Windows (`winmm.dll`) com seleção dinâmica de portas MIDI In/Out e mensagens SysEx oficiais Casio de 263/264 bytes.

---

## 🛠️ Compilação e Execução

### Pré-requisitos
- Windows 10 ou 11 (x64)
- Visual Studio 2022 / Build Tools com suporte a C++20
- CMake 3.20+
- Git com submódulos (`git clone --recursive` ou `git submodule update --init --recursive`)
- *(Opcional)* NSIS 3.x para gerar o instalador Windows

### Compilação rápida e Testes
```bat
build.bat
```
Gera `build\Release\CZ3000SysEx.exe` e `build\Release\test_sysex.exe`, executando todos os testes unitários.

### Gerar Instalador
```bat
build_installer.bat
```

---

## 📁 Estrutura do Projeto

```
CZSYSEX/
├── docs/                  # Capturas de tela e recursos de documentação
├── external/
│   └── imgui/             # Submódulo Dear ImGui + backends Win32/DX11
├── installer/             # Script do instalador NSIS
├── src/
│   ├── core/              # Modelo de dados, matemática, i18n, codec SysEx e cache de banco
│   ├── midi/              # Driver MIDI Windows Multimedia (WinMM)
│   ├── ui/                # LCD, Tutorial, Manual, Gráfico de Envelopes, Librarian e Painel
│   └── main.cpp           # Janela Win32, DirectX 11 e loop principal
├── tests/
│   └── test_sysex.cpp     # Testes unitários de SysEx, banco e cache
├── AGENTS.md              # Diretrizes para colaboradores e verificação CI
├── CMakeLists.txt
├── build.bat
└── build_installer.bat
```

---

## 📄 Licença

Distribuído sob a **GNU General Public License v3.0 (GPLv3)**. Consulte [LICENSE](LICENSE) para mais detalhes.
