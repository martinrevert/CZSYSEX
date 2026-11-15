# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

🌐 **Idioma:** [English](README.md) · **Español** · [Italiano](README.it.md) · [Deutsch](README.de.md) · [日本語](README.ja.md) · [简体中文](README.zh.md) · [Português (BR)](README.pt-BR.md) · [Français](README.fr.md)

Aplicación nativa moderna en **C++20** con interfaz gráfica acelerada por hardware (**DirectX 11 + Dear ImGui**) para el sintetizador **Casio CZ-3000** (compatible con la familia Casio CZ: CZ-101, CZ-1000, CZ-3000, CZ-5000 y CZ-1).

Portado y expandido a partir del proyecto Python `kasploosh` (`czsysexy` / `czcommon`).

![Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor](docs/screenshot.jpg)

---

## 🎹 Características Principales

1. **Simulación Fidedigna del Hardware Casio CZ-3000**:
   - Chasis retro en gris carbón con franjas clásicas Casio (cian, azul oscuro y rojo).
   - **Pantalla LCD 16×2 HD44780**: Matriz 5×7 verde oliva con cursor parpadeante y páginas reales de edición (`Play`, `Line`, `Vibrato`, `Detune`, `DCO`, `DCW`, `DCA`).
   - **Botonera Táctil Casio con LEDs de Estado**: `COMPARE/RECALL`, `INIT`, `WRITE`, `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`, Líneas 1/2, `Data Entry` (`UP`, `DOWN`, `< CUR`, `CUR >`) y selector de Bancos `A`–`D` / Voces `1`–`8` (64 voces).
   - **Teclado de Audición en Vivo de 6 Octavas**: Piano virtual interactivo (**C1** a **C7**, 73 teclas) para tocar en tiempo real por MIDI.

2. **Gráfico Interactivo de Envolventes de 8 Pasos**:
   - Visualización y edición en vivo de las curvas **DCO** (tono, cian), **DCW** (brillo/PD, ámbar) y **DCA** (volumen, verde lima), por separado o superpuestas (`ALL`).
   - Marcadores visuales de `SUS` (Sustain) y `END` (Fin).
   - Deslizadores de `Rate` (0–99) y `Level` (0–99) para los 8 pasos.

3. **Gestor de Banco Dual de 64 Voces (32 Presets + 32 Memorias de Usuario)**:
   - Pestañas **PRESET BANK (32 Fijos)** y **MEMORY BANK (32 Usuario)** con formato `*MEMORY* A-1` fiel al sintetizador y renombrado personalizado con clic derecho (conservado al exportar/importar `.syx` formato CZ-1 de 296 bytes).
   - **Descarga en Lote y Caché de Presets**: Descarga los 32 Presets (guardados en caché local) o las 32 Memorias de una sola vez.
   - **Auto-Fetch SysEx**: Solicita automáticamente el volcado SysEx al hacer clic en un Preset o Memoria.
   - **Randomizador (`Aleatorio` / `Randomize`)**: Aleatoriza todos los parámetros del sintetizador en la memoria actual y los envía en vivo al búfer de edición (`0x60`) para escuchar el resultado al instante.
   - Enviar al Búfer (`0x60`), Guardar en Memoria del Synth, Enviar Banco completo (32), Copiar/Pegar y abrir/guardar archivos `.syx`.

4. **Multi-Idioma, Guía PD Interactiva y Manual Oficial (8 Idiomas)**:
   - Cambio de idioma en caliente (**English**, **Español**, **Italiano**, **Deutsch**, **日本語**, **简体中文**, **Português (Brasil)**, **Français**) con soporte completo de fuentes CJK.
   - **Modo Tutorial (`[?] MODO TUTORIAL: ON/OFF`)**: Tooltips didácticos y ventana interactiva de **Guía de Distorsión de Fase**.
   - **Manual Oficial del Casio CZ-3000**: Vista integrada (`Vistas → Manual Oficial CZ-3000`) con los 9 capítulos del manual oficial, tabla de los 32 Presets, tabla `INITIALIZE` y Tabla de Implementación MIDI en los 8 idiomas.

5. **Motor MIDI Nativo de Baja Latencia**:
   - Basado en `winmm.dll` de Windows con selección de puertos MIDI In/Out y tramas SysEx oficiales Casio de 263/264 bytes.

---

## 🛠️ Compilación y Ejecución

### Requisitos
- Windows 10 u 11 (x64)
- Visual Studio 2022 / Build Tools con soporte C++20
- CMake 3.20+
- Git con submódulos (`git clone --recursive` o `git submodule update --init --recursive`)
- *(Opcional)* NSIS 3.x para generar el instalador

### Compilación rápida y pruebas
```bat
build.bat
```
Genera `build\Release\CZ3000SysEx.exe` y `build\Release\test_sysex.exe`, y ejecuta todas las pruebas unitarias.

### Generar instalador
```bat
build_installer.bat
```

---

## 📄 Licencia

Distribuido bajo la licencia **GNU General Public License v3.0 (GPLv3)**. Consulta [LICENSE](LICENSE) para más detalles.
