# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Idioma:** [English](README.md) · **Español** · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

Aplicación de escritorio nativa moderna en **C++20** con interfaz gráfica acelerada por hardware (**DirectX 11 + Dear ImGui**) para el sintetizador **Casio CZ-3000** (compatible con toda la familia Casio CZ: CZ-101, CZ-1000, CZ-3000, CZ-5000 y CZ-1).

Portado y expandido a partir del proyecto Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Características Principales

1. **Simulación Fidedigna del Hardware Casio CZ-3000**:
   - Chasis retro en gris carbón con franjas clásicas Casio (cian, azul oscuro y rojo).
   - **Pantalla LCD 16×2 HD44780**: Matriz de puntos 5×7 verde fósforo con cursor parpadeante de hardware y páginas reales de edición (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Botonera Táctil Casio y LEDs de Estado**:
     - *Programmer*: `COMPARE/RECALL`, `INIT`, `WRITE`.
     - *Selección de Parámetro*: `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`.
     - *Líneas de Voz*: Línea 1 y Línea 2.
     - *Data Entry*: `UP`, `DOWN`, `< CUR`, `CUR >`.
     - *Bancos y Voces*: Bancos de Preset y Memory `A`–`D` y Ranuras `1`–`8` (64 voces en total).
   - **Teclado de Audición en Vivo de 6 Octavas**: Piano virtual interactivo (**C1** a **C7**, 73 teclas) para audición MIDI en tiempo real.

2. **Gráfico Interactivo de Envolventes de 8 Pasos**:
   - Visualización y edición en tiempo real de las curvas **DCO** (tono, cian), **DCW** (brillo/PD, ámbar) y **DCA** (volumen, verde lima), individualmente o superpuestas (`ALL`).
   - Marcadores visuales de `SUS` (Sustain) y `END` (Fin).
   - Deslizadores de `Rate` (0–99) y `Level` (0–99) para los 8 pasos.

3. **Gestor de Banco Dual de 64 Voces (32 Presets + 32 Memorias de Usuario)**:
   - Pestañas **PRESET BANK (32 Fijos)** y **MEMORY BANK (32 Usuario)** con formato `*MEMORY* A-1` fiel al hardware y renombrado personalizado con clic derecho (preservado mediante exportación/importación `.syx` CZ-1 de 296 bytes).
   - **Descarga en Lote y Caché de Presets**: Descarga los 32 Presets (guardados en caché local para no tener que leerlos en cada sesión) o las 32 Memorias en un solo clic.
   - **Auto-Fetch SysEx**: Solicita automáticamente la ranura seleccionada al sintetizador al hacer clic en un Preset o Memoria.
   - **Randomizador de Patches (`Aleatorio` / `Randomize`)**: Aleatoriza todos los parámetros del sintetizador (`Line Select`, `Octave`, `Detune`, `Vibrato`, ondas, modulación y envolventes de 8 etapas DCO/DCW/DCA) en la memoria actual y los envía en vivo al búfer de edición (`0x60`).
   - Enviar al Búfer de Edición (`0x60`), Guardar en Memoria del Synth, Enviar Banco completo de 32 patches, Copiar/Pegar entre ranuras y cargar/guardar archivos `.syx`.

4. **Interfaz Multi-Idioma, Guía PD Interactiva y Manual Oficial (8 Idiomas)**:
   - Cambio de idioma en tiempo real (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**) con soporte de fuentes CJK.
   - **Modo Tutorial (`[?] MODO TUTORIAL: ON/OFF`)**: Tooltips al pasar el cursor y ventana interactiva de **Guía de Distorsión de Fase** explicando DCO, DCW, DCA, envolventes de 8 pasos, ondas, modulación Ring/Noise y SysEx.
   - **Vista del Manual Oficial del CZ-3000**: Manual de referencia integrado de 9 capítulos (`Vistas → Manual Oficial CZ-3000`) con tabla de los 32 Presets, valores de `INITIALIZE` y Tabla de Implementación MIDI en los 8 idiomas.

5. **Motor MIDI Nativo de Baja Latencia y Plugin VST3 (DAW / Ableton Live)**:
   - Basado en la API multimedia nativa de Windows (`winmm.dll`) con selección en caliente de puertos MIDI In/Out y tramas SysEx oficiales Casio de 263/264 bytes.
   - **Plugin MIDI VST3 (`CZ3000SysEx.vst3`)**: Instalable en `C:\Program Files\Common Files\VST3` para control inmediato de los 64 parches desde pistas MIDI de DAWs (Ableton Live, Cubase, Reaper) con sincronización automática de bancos, envío de cambios de programa y persistencia en el proyecto del DAW.

---

## 🛠️ Compilación y Ejecución

### Requisitos
- Windows 10 u 11 (x64)
- Visual Studio 2022 / Build Tools con soporte C++20
- CMake 3.20+
- Git con submódulos (`git clone --recursive` o `git submodule update --init --recursive`)
- *(Opcional)* NSIS 3.x para empaquetar el instalador de Windows

### Compilación rápida y Pruebas
```bat
build.bat
```
Genera `build\Release\CZ3000SysEx.exe`, `build\Release\CZ3000SysEx.vst3` y `build\Release\test_sysex.exe`, y ejecuta todas las pruebas unitarias.

### Generar Instalador
```bat
build_installer.bat
```

---

## 📁 Estructura del Proyecto

```
CZSYSEX/
├── docs/                  # Capturas de pantalla y recursos de documentación
├── external/
│   └── imgui/             # Submódulo Dear ImGui + backends Win32/DX11
├── installer/             # Script de instalador NSIS
├── src/
│   ├── core/              # Modelo de datos, matemáticas, i18n, codec SysEx y caché de banco
│   ├── midi/              # Driver MIDI Windows Multimedia (WinMM)
│   ├── ui/                # LCD, Tutorial, Manual, Gráfico de Envolventes, Librarian y Panel
│   ├── vst3/              # Procesador, vista e inicializador del plugin VST3
│   └── main.cpp           # Ventana Win32, DirectX 11 y bucle principal
├── tests/
│   └── test_sysex.cpp     # Pruebas unitarias de SysEx, banco y caché
├── AGENTS.md              # Guías para colaboradores y verificación CI
├── CMakeLists.txt
├── build.bat
└── build_installer.bat
```

---

## 📄 Licencia

Distribuido bajo la **GNU General Public License v3.0 (GPLv3)**. Consulta [LICENSE](LICENSE) para más detalles.
