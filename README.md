# Casio CZ-3000 SysEx Librarian & Hardware Synthesizer Editor (C++)

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

Aplicación nativa moderna en **C++20** con interfaz gráfica acelerada por hardware (**DirectX 11 + Dear ImGui**) para el sintetizador **Casio CZ-3000** (compatible con toda la familia Casio CZ: CZ-101, CZ-1000, CZ-3000, CZ-5000 y CZ-1).

Portado y expandido a partir del proyecto Python `kasploosh` (`czsysexy` / `czcommon`).

---

## 🎹 Características Principales

1. **Simulación Fidedigna del Hardware Casio CZ-3000**:
   - Chasis con diseño retro en gris carbón y franjas características Casio (cian, azul oscuro y rojo).
   - **Pantalla LCD 16×2 HD44780**: Matriz de puntos 5×7 con fósforo verde/oliva brillante, cursor parpadeante de hardware y modos de edición reales (Play, DCO, DCW, DCA, Vibrato, Detune, Line Select).
   - **Botonera Táctil Casio con LEDs de Estado**:
     - *Programmer*: `COMPARE`, `RECALL`, `INIT`, `WRITE`.
     - *Selección de Parámetro*: `PLAY`, `LINE`, `VIB`, `DETUNE`, `DCO`, `DCW`, `DCA`.
     - *Líneas de Voz*: Línea 1 y Línea 2.
     - *Data Entry*: `UP`, `DN`, `< CUR`, `CUR >`.
     - *Bancos y Números*: Bancos A, B, C, D y Ranuras 1 a 8 (32 memorias de hardware).
   - **Teclado de Audición en Vivo de 6 Octavas Fijas**: Teclado de piano virtual interactivo continuo de 6 octavas completas (**C1** a **C7**, 73 teclas: 43 blancas y 30 negras) para audición inmediata vía MIDI en tiempo real.

2. **Modo Tutorial Interactivo (PD Guide & Beginner Tooltips)**:
   - Botón destacado en el encabezado: `[?] MODO TUTORIAL: ON / OFF`.
   - Tooltips enriquecidos en español con explicaciones sin jerga técnica para principiantes:
     - **DCO**: qué es y cómo controla el tono y la afinación.
     - **DCW (El 'Filtro' del CZ)**: explicación de cómo sustituye al filtro analógico tradicional (VCF) mediante la deformación de fase de una onda senoidal.
     - **DCA**: dinámica de volumen y envolvente de amplitud.
     - **Envolventes de 8 Pasos**: por qué son más expresivas que el ADSR tradicional de 4 etapas, cómo funcionan los puntos de sostenido (`SUS`) y fin (`END`).
     - **Estructura de Líneas**: `1+1'` (duplicación con chorus estéreo) y `1+2'` (capas de dos instrumentos diferentes).
     - **Modulación en Anillo (Ring Mod) y Ruido (Noise)**.
   - Ventana de **Guía Didáctica** accesible con un solo clic (`Guía`), con artículos explicativos por temas.

3. **Gráfico Interactivo de Envolventes de 8 Pasos**:
   - Visualización y edición en tiempo real de las curvas de DCO (azul cian), DCW (amarillo anaranjado) y DCA (verde lima).
   - Marcadores visuales de `SUS` (Sustain) y `END` (Fin de ciclo).
   - Sliders de Rate (Velocidad: 0-99) y Level (Nivel: 0-99) por cada paso (1 a 8).

4. **Gestor de Banco Integrado (Librarian 32 Ranuras)**:
   - Disposición optimizada en el panel superior a la derecha del LCD y controles.
   - Selector de banco (A, B, C, D) con visualización de la lista de 8 sonidos.
   - Renombrado rápido de sonidos con clic derecho.
   - Enviar al Búfer de Edición del CZ (`Recall / Slot 0x60`) para audición instantánea.
   - Guardar permanentemente en la memoria interna del sintetizador.
   - Enviar el banco entero de 32 patches con espaciado temporal seguro de microcontrolador.
   - Copiar y pegar parches entre ranuras.

5. **Motor MIDI Nativo de Baja Latencia**:
   - Utiliza la API multimedia nativa de Windows (`winmm.dll`).
   - Detección automática y conmutación de puertos MIDI In y MIDI Out.
   - Transmisión y recepción de tramas SysEx oficiales de Casio de 264 bytes.

---

## 🛠️ Compilación y Ejecución

### Requisitos:
- Windows 10 u 11 (x64)
- Visual Studio 2022 o Visual Studio Build Tools con soporte C++20
- CMake 3.20+
- (Opcional) NSIS 3.x para generar el instalador ejecutable

### Compilación rápida:
En una consola de comandos o PowerShell en la raíz del proyecto:
```bat
build.bat
```

El ejecutable optimizado se generará en:
```
build\Release\CZ3000SysEx.exe
```

### Generación del Instalador:
```bat
build_installer.bat
```
Genera `dist\CZ3000SysEx_Setup_v1.0.0.exe`.

---

## 📁 Estructura del Código

```
CZSYSEX/
├── build/                 # Directorio de compilación
├── dist/                  # Paquete instalador final (NSIS)
├── external/
│   └── imgui/             # Dear ImGui + Win32/DX11 backends
├── installer/
│   └── cz3000_installer.nsi # Script de empaquetado NSIS
├── src/
│   ├── core/              # Modelo de datos, matemáticas y codec SysEx
│   │   ├── cz_types.h     # Estructuras de Parche, Envolvente, Líneas, etc.
│   │   ├── cz_math.h/.cpp # Fórmulas de conversión no lineal (DCO/DCW/DCA)
│   │   ├── cz_sysex.h/.cpp# Empaquetado de nibbles Casio y parser de .syx
│   │   └── cz_bank.h/.cpp # Gestión de banco de 32 ranuras A1-D8
│   ├── midi/
│   │   └── cz_midi.h/.cpp # Driver MIDI Windows Multimedia (WinMM)
│   ├── ui/
│   │   ├── cz_lcd.h/.cpp  # Simulación de pantalla LCD HD44780 de 16x2
│   │   ├── cz_tutorial.h/.cpp # Modo Tutorial y glosario interactivo PD
│   │   ├── cz_envelope_graph.h/.cpp # Gráfico interactivo de 8 pasos
│   │   ├── cz_librarian.h/.cpp      # Gestor de 32 patches
│   │   └── cz_panel.h/.cpp          # Chasis y controles del CZ-3000
│   └── main.cpp           # Ventana Win32, inicialización DirectX 11 y bucle principal
├── tests/
│   └── test_sysex.cpp     # Pruebas unitarias de ida y vuelta SysEx
├── CMakeLists.txt
├── build.bat
├── build_installer.bat
├── LICENSE
└── README.md
```

---

## 📄 Licencia

Este proyecto está distribuido bajo los términos de la **GNU General Public License v3.0 (GPLv3)**.

Consulta el archivo [LICENSE](LICENSE) para conocer todos los términos y condiciones de copia, distribución y modificación.

