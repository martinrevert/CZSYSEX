#include "cz_manual.h"
#include "../core/cz_i18n.h"
#include <imgui.h>

namespace cz {

CzManual& CzManual::instance() {
    static CzManual inst;
    return inst;
}

CzManual::CzManual() {
    init_sections();
}

void CzManual::init_sections() {
    auto add = [&](Language lang, const char* id, const char* title, const char* subtitle, const char* body) {
        sections_[static_cast<size_t>(lang)].push_back({id, title, subtitle, body});
    };

    // =========================================================================
    // 1. ENGLISH (En)
    // =========================================================================
    add(Language::En, "01_general",
        "1. Overview & General Guide",
        "3 Performance Modes & Panel Layout (Manual pp. 2-5)",
        "The Casio CZ-3000 is a 61-key, 16-voice polyphonic Phase Distortion (PD) digital synthesizer.\n\n"
        "THREE PERFORMANCE MODES:\n"
        "• NORMAL: Plays a single selected Tone across the entire keyboard (16 voices with 1 Line, 8 voices with 2 Lines).\n"
        "• TONE MIX: Layers two Tones simultaneously (4-note polyphony; 8-note if both use 1 Line). Independent balance levels LEV1/LEV2.\n"
        "• KEY SPLIT: Splits the keyboard into LOWER and UPPER zones at a programmable Split Point (keys 1-60) with independent effects.\n\n"
        "PANEL SECTIONS (1-14):\n"
        "1. TOTAL CONTROL: Power switch, Master Volume slider.\n"
        "2. MODE SELECT: NORMAL, TONE MIX, KEY SPLIT, and SPLIT POINT button.\n"
        "3. SOLO: Switches between polyphonic and monophonic (single-note) playback.\n"
        "4. MIDI: Channel selection (CH), Keyboard Channel (KBCH), Program Change Enable/Disable (PRG), P.MODE / M.MODE.\n"
        "5. PROGRAMMER: BANK (A-D), NUMBER (1-8), PRESET / MEMORY / CARTRIDGE, WRITE, COMPARE/RECALL, INITIALIZE.\n"
        "6. LCD DISPLAY: 16x2 character backlit display showing mode, tone, parameter, and value.\n"
        "7. DATA ENTRY: VALUE Up/Down buttons and CURSOR Left/Right buttons.\n"
        "8. CARTRIDGE: RA-3 RAM Cartridge slot (stores 32 extra tones) + SAVE / LOAD.\n"
        "9. EFFECT: PORTAMENTO, GLIDE, BEND RANGE, MODULATION, TUNE, TRANSPOSE.\n"
        "10. PARAMETER: DCO 1/2, DCW 1/2, DCA 1/2, VIBRATO, OCTAVE, DETUNE, LINE SELECT, RING, NOISE, ENV STEP, SUSTAIN, END.\n"
        "11. PITCH BEND & MODULATION WHEELS: Real-time pitch bend and vibrato/modulation depth.\n"
        "12. REAR PANEL: Stereo LINE OUT (L/R), PHONES, SUSTAIN pedal, PORTAMENTO footswitch, MIDI IN / OUT / THRU.\n"
        "13. MEMORY PROTECT: Rear-panel switch (ON/OFF) to prevent accidental overwriting of Memory banks.");

    add(Language::En, "02_presets",
        "2. Normal Mode: Preset & Memory",
        "32 Internal Presets & 32 User Memories (Manual pp. 6-8)",
        "In NORMAL mode, select a voice by choosing a Bank (A, B, C, D) and a Number (1-8).\n\n"
        "• PRESET (32 Voices): Read-only factory ROM sounds (A-1 to D-8). Ideal as starting points for editing.\n"
        "• MEMORY (32 Voices): User-programmable RAM slots (A-1 to D-8) backed up by 3x AA batteries.\n"
        "  Note: On the CZ-3000 hardware, Memory slots do not store custom text names (the LCD shows *MEMORY* A-1).\n\n"
        "In the table below, voices marked with (*) use 1 DCO Line (16-note polyphony); unmarked voices use 2 Lines (8-note polyphony):");

    add(Language::En, "03_effects",
        "3. Effect & Overall Control",
        "Portamento, Glide, Bend, Mod, Tune, Transpose (Manual pp. 9-10)",
        "The EFFECT section provides global expressive controls per zone/tone:\n\n"
        "• PORTAMENTO (ON/OFF, TIME 00-99): Smoothly slides pitch from the previous note to the next. 00 = fastest, 99 = slowest.\n"
        "• GLIDE (ON/OFF, NOTE -24 to +24, TIME 00-99): Triggers a fixed-interval pitch sweep at every key attack.\n"
        "• BEND RANGE (00-12 semitones): Sets the maximum pitch bend wheel excursion up/down (default ±2 or ±12).\n"
        "• MODULATION (DEPTH 00-99): Sets the maximum vibrato depth controlled by the Modulation wheel.\n"
        "• MASTER TUNE (-100 to +100 cents): Fine-tunes overall pitch around A4 = 442 Hz.\n"
        "• KEY TRANSPOSE (G to F#): Shifts keyboard pitch in semitones over ±1 octave.\n"
        "• SOLO (ON/OFF): Forces monophonic mode with last-note priority.");

    add(Language::En, "04_pd_env",
        "4. PD Sound Source & Envelopes",
        "Phase Distortion Principle & 8-Step Envelopes (Manual pp. 11-15)",
        "PHASE DISTORTION (PD) PRINCIPLE:\n"
        "Instead of filtering harmonically rich waveforms (subtractive synthesis), PD reads a digital cosine wave from a lookup table and distorts the phase angle speed during each cycle.\n"
        "• DCO sets the fundamental pitch and waveform shape.\n"
        "• DCW controls the amount of phase angle distortion (brightness / harmonic content).\n"
        "• DCA controls the final output amplitude (volume).\n\n"
        "8-STEP ENVELOPE GENERATOR:\n"
        "Each block (DCO, DCW, DCA) has an independent 8-step envelope.\n"
        "• RATE (00-99): Speed of transition to the step's target level (99 = instant, 00 = slowest).\n"
        "• LEVEL (00-99): Target amplitude, distortion depth, or pitch offset for that step.\n"
        "• SUSTAIN (1-8 or OFF): Holds at this step while the key remains pressed. When released, jumps to the next step toward END.\n"
        "• END (1-8): Marks the final step of the envelope. Steps beyond END are ignored.");

    add(Language::En, "05_params",
        "5. Sound Parameters & Initialize",
        "Waveforms, Line Select, Detune, Vibrato, Key Follow (Manual pp. 16-24)",
        "WAVEFORMS (W1 & W2):\n"
        "8 base waveforms can be paired (W1 + W2) to produce 33 total combinations:\n"
        "1: Sawtooth | 2: Square | 3: Pulse | 4: Double Sine | 5: Saw-Pulse | 6: Resonance I | 7: Resonance II | 8: Resonance III.\n\n"
        "KEY FOLLOW (0-9):\n"
        "• DCW Key Follow: Reduces upper-register brightness across the keyboard.\n"
        "• DCA Key Follow: Shortens envelope decay on higher notes (like acoustic piano/harp).\n\n"
        "VIBRATO (LFO):\n"
        "• WAVE: 1=Triangle, 2=Saw Up, 3=Saw Down, 4=Square.\n"
        "• DELAY (00-99), RATE (00-99), DEPTH (00-99).\n\n"
        "LINE SELECT & MODULATION:\n"
        "• 1 or 2: Single oscillator line (16-note polyphony).\n"
        "• 1+2' or 1+1': Dual oscillator lines (8-note polyphony). Line 2'/1' can be detuned via +/- OCTAVE (0-3), NOTE (0-11), FINE (0-60).\n"
        "• RING MOD: Multiplies Line 1 and Line 2' for metallic bells/gongs.\n"
        "• NOISE MOD: Mixes digital noise into phase for breathy/percussive sounds.\n\n"
        "INITIALIZE: Resets the Edit Buffer to a neutral single-sine starting patch:");

    add(Language::En, "06_storage",
        "6. Write, Compare & Cartridge",
        "Saving Patches & RA-3 RAM Cartridge (Manual pp. 25-27)",
        "• COMPARE / RECALL: Toggles between your currently edited sound in the Edit Buffer and the original saved sound (or recalls the last edited buffer).\n"
        "• WRITE: Saves the current Edit Buffer into a MEMORY slot (A-1 to D-8).\n"
        "  IMPORTANT: The rear-panel MEMORY PROTECT switch must be set to OFF before pressing WRITE.\n"
        "• RA-3 RAM CARTRIDGE: Optional external cartridge storing 32 additional voices (Banks A-D).\n"
        "  - SAVE: Copies Internal Memory (A,B or C,D) to Cartridge.\n"
        "  - LOAD: Loads Cartridge (A,B or C,D) into Internal Memory.\n"
        "  - Uses a BR-2016 lithium battery for data retention.");

    add(Language::En, "07_mix_split",
        "7. Tone Mix & Key Split Modes",
        "Layering & Keyboard Splitting (Manual pp. 28-31)",
        "TONE MIX MODE:\n"
        "• Layers two Tones (Tone 1 + Tone 2) across the full keyboard.\n"
        "• Balance levels LEV1 (1-15) and LEV2 (1-15) control the mix.\n"
        "• Polyphony: 4 notes when both Tones use 2 Lines; up to 8 notes if both use 1 Line.\n\n"
        "KEY SPLIT MODE:\n"
        "• Divides the 61 keys into LOWER and UPPER zones.\n"
        "• Press SPLIT POINT and play any key (1-60) to set the split boundary.\n"
        "• Independent LOWER LEV (1-15), UPPER LEV (1-15), Portamento, Glide, and Transpose per zone.");

    add(Language::En, "08_midi",
        "8. MIDI & Implementation Chart",
        "P.MODE, M.MODE, Multi-Timbral & SysEx (Manual pp. 32-39)",
        "MIDI MODES:\n"
        "• P.MODE (Performance Mode): Standard single/split/mix MIDI operation. Set KBCH (Keyboard Tx/Rx channel) and PRG=ENA/DIS.\n"
        "• M.MODE (Multi-Timbral Mode): Turns the CZ-3000 into a 4-channel (or up to 8-voice) multi-timbral MIDI expander, assigning independent Tones to consecutive MIDI channels.\n\n"
        "PROGRAM CHANGE MAPPING:\n"
        "• Program 0-31 -> Internal PRESET A-1 to D-8\n"
        "• Program 32-63 -> Internal MEMORY A-1 to D-8\n"
        "• Program 64-95 -> CARTRIDGE A-1 to D-8\n\n"
        "SYSEX DATA FORMAT:\n"
        "• Manufacturer ID: 44H (Casio). Patch dump = 263/264 bytes with parameters encoded as 4-bit nibbles (low nibble first).");

    add(Language::En, "09_specs",
        "9. Care & Specifications",
        "Maintenance & Technical Specs (Manual pp. 40-42)",
        "CARE & PRECAUTIONS:\n"
        "1. Avoid extreme heat, direct sunlight, humidity, or dust.\n"
        "2. Do not drop or subject the unit to strong impact.\n"
        "3. Never use benzine, thinner, or chemical cleaners on the chassis.\n"
        "4. Replace the 3x AA memory backup batteries every ~1-2 years with power ON to avoid losing RAM patches.\n"
        "5. Check the rear voltage selector (100V/120V/220V/240V) before plugging in.\n"
        "6. Keep memory protect ON during live performance.\n\n"
        "SPECIFICATIONS:\n"
        "• Keyboard: 61 standard keys (C1-C6)\n"
        "• Sound Source: PD (Phase Distortion) System\n"
        "• Polyphony: 16 voices (1 Line) / 8 voices (2 Lines)\n"
        "• Memory: 32 Preset + 32 Internal Memory + 32 RA-3 Cartridge\n"
        "• Jacks: LINE OUT L/R (1/4\"), PHONES, SUSTAIN, PORTAMENTO, MIDI IN/OUT/THRU\n"
        "• Power: AC 100/120/220/240V, 26W; Backup: 3x AA batteries\n"
        "• Dimensions & Weight: 1025 x 341 x 125 mm, 11.7 kg");

    // =========================================================================
    // 2. SPANISH (Es)
    // =========================================================================
    add(Language::Es, "01_general",
        "1. Visión General y Guía del Panel",
        "3 Modos de Interpretación y Controles (Manual pp. 2-5)",
        "El Casio CZ-3000 es un sintetizador digital de 61 teclas y 16 voces de polifonía basado en Distorsión de Fase (PD).\n\n"
        "TRES MODOS DE INTERPRETACIÓN:\n"
        "• NORMAL: Toca un único Timbre en todo el teclado (16 voces con 1 Línea, 8 voces con 2 Líneas).\n"
        "• TONE MIX: Superpone dos Timbres simultáneamente (4 voces de polifonía; 8 si ambos usan 1 Línea) con balance independiente LEV1/LEV2.\n"
        "• KEY SPLIT: Divide el teclado en zonas LOWER y UPPER en un Split Point programable (teclas 1-60) con efectos independientes.\n\n"
        "SECCIONES DEL PANEL (1-14):\n"
        "1. TOTAL CONTROL: Interruptor de encendido y deslizador Master Volume.\n"
        "2. MODE SELECT: Botones NORMAL, TONE MIX, KEY SPLIT y SPLIT POINT.\n"
        "3. SOLO: Alterna entre modo polifónico y monofónico (una sola nota).\n"
        "4. MIDI: Selección de canal (CH), Keyboard Channel (KBCH), Program Change (PRG), P.MODE / M.MODE.\n"
        "5. PROGRAMMER: BANK (A-D), NUMBER (1-8), PRESET / MEMORY / CARTRIDGE, WRITE, COMPARE/RECALL, INITIALIZE.\n"
        "6. PANTALLA LCD: Display retroiluminado de 16x2 caracteres con modo, timbre, parámetro y valor.\n"
        "7. DATA ENTRY: Botones VALUE Arriba/Abajo y CURSOR Izquierda/Derecha.\n"
        "8. CARTRIDGE: Ranura para cartucho RAM RA-3 (32 sonidos extra) + SAVE / LOAD.\n"
        "9. EFFECT: PORTAMENTO, GLIDE, BEND RANGE, MODULATION, TUNE, TRANSPOSE.\n"
        "10. PARAMETER: DCO 1/2, DCW 1/2, DCA 1/2, VIBRATO, OCTAVE, DETUNE, LINE SELECT, RING, NOISE, ENV STEP, SUSTAIN, END.\n"
        "11. RUEDAS PITCH BEND Y MODULATION: Inflexión tonal y profundidad de vibrato en tiempo real.\n"
        "12. PANEL TRASERO: Salidas estéreo LINE OUT (L/R), PHONES, pedal SUSTAIN, pedal PORTAMENTO, MIDI IN / OUT / THRU.\n"
        "13. MEMORY PROTECT: Interruptor trasero (ON/OFF) para evitar sobrescribir accidentalmente las memorias.");

    add(Language::Es, "02_presets",
        "2. Modo Normal: Preset y Memory",
        "32 Presets Internos y 32 Memorias de Usuario (Manual pp. 6-8)",
        "En modo NORMAL, selecciona un sonido eligiendo un Banco (A, B, C, D) y un Número (1-8).\n\n"
        "• PRESET (32 Voces): Sonidos de fábrica en ROM de solo lectura (A-1 a D-8). Ideales como punto de partida.\n"
        "• MEMORY (32 Voces): Ranuras RAM programables por el usuario (A-1 a D-8) respaldadas por 3 pilas AA.\n"
        "  Nota: El hardware CZ-3000 no guarda nombres de texto en Memory (su pantalla muestra *MEMORY* A-1).\n\n"
        "En la tabla inferior, los timbres marcados con (*) usan 1 Línea DCO (16 voces); los demás usan 2 Líneas (8 voces):");

    add(Language::Es, "03_effects",
        "3. Efectos y Control Global",
        "Portamento, Glide, Bend, Mod, Tune, Transpose (Manual pp. 9-10)",
        "La sección EFFECT ofrece controles expresivos globales por zona o timbre:\n\n"
        "• PORTAMENTO (ON/OFF, TIME 00-99): Desliza suavemente el tono de una nota a la siguiente. 00 = más rápido, 99 = más lento.\n"
        "• GLIDE (ON/OFF, NOTE -24 a +24, TIME 00-99): Dispara un barrido tonal fijo al pulsar cada tecla.\n"
        "• BEND RANGE (00-12 semitonos): Rango máximo de la rueda Pitch Bend arriba/abajo.\n"
        "• MODULATION (DEPTH 00-99): Profundidad máxima de vibrato controlada por la rueda Modulation.\n"
        "• MASTER TUNE (-100 a +100 cents): Afinación fina global en torno a La4 = 442 Hz.\n"
        "• KEY TRANSPOSE (G a F#): Transporta el teclado en semitonos (±1 octava).\n"
        "• SOLO (ON/OFF): Activa modo monofónico con prioridad a la última nota.");

    add(Language::Es, "04_pd_env",
        "4. Fuente PD y Envolventes de 8 Pasos",
        "Principio de Distorsión de Fase y Envolventes (Manual pp. 11-15)",
        "PRINCIPIO DE DISTORSIÓN DE FASE (PD):\n"
        "En lugar de filtrar ondas ricas en armónicos (síntesis sustractiva), el motor PD lee una onda coseno digital y distorsiona la velocidad de su ángulo de fase en cada ciclo.\n"
        "• DCO define la afinación fundamental y forma de onda.\n"
        "• DCW controla la cantidad de distorsión de fase (brillo y armónicos).\n"
        "• DCA controla la amplitud final (volumen).\n\n"
        "GENERADOR DE ENVOLVENTES DE 8 ETAPAS:\n"
        "Cada bloque (DCO, DCW, DCA) posee una envolvente independiente de 8 pasos.\n"
        "• RATE (00-99): Velocidad de transición hacia el nivel objetivo (99 = instantáneo, 00 = más lento).\n"
        "• LEVEL (00-99): Nivel objetivo de amplitud, distorsión o desplazamiento tonal.\n"
        "• SUSTAIN (1-8 o OFF): Pausa la envolvente mientras mantienes la tecla pulsada; al soltarla salta hacia END.\n"
        "• END (1-8): Marca la etapa final de la envolvente. Los pasos posteriores se ignoran.");

    add(Language::Es, "05_params",
        "5. Parámetros de Sonido e Initialize",
        "Ondas, Line Select, Detune, Vibrato, Key Follow (Manual pp. 16-24)",
        "FORMAS DE ONDA (W1 y W2):\n"
        "8 ondas base combinables (W1 + W2) generan 33 espectros totales:\n"
        "1: Sawtooth | 2: Square | 3: Pulse | 4: Double Sine | 5: Saw-Pulse | 6: Resonance I | 7: Resonance II | 8: Resonance III.\n\n"
        "KEY FOLLOW (0-9):\n"
        "• DCW Key Follow: Reduce el brillo en notas agudas a lo largo del teclado.\n"
        "• DCA Key Follow: Acorta el decaimiento en notas agudas (como un piano o arpa acústica).\n\n"
        "VIBRATO (LFO):\n"
        "• WAVE: 1=Triangular, 2=Sierra Arriba, 3=Sierra Abajo, 4=Cuadrada.\n"
        "• DELAY (00-99), RATE (00-99), DEPTH (00-99).\n\n"
        "LINE SELECT Y MODULACIÓN:\n"
        "• 1 o 2: 1 línea de oscilador (16 voces).\n"
        "• 1+2' o 1+1': 2 líneas (8 voces). La Línea 2'/1' se desafina con +/- OCTAVE (0-3), NOTE (0-11), FINE (0-60).\n"
        "• RING MOD: Multiplica Línea 1 y 2' para campanas metálicas y gongs.\n"
        "• NOISE MOD: Mezcla ruido digital en la fase para soplo o percusión.\n\n"
        "INITIALIZE: Reinicia el Edit Buffer a una onda senoidal neutra:");

    add(Language::Es, "06_storage",
        "6. Write, Compare y Cartucho RA-3",
        "Guardar Patches y Cartucho RAM (Manual pp. 25-27)",
        "• COMPARE / RECALL: Alterna entre tu sonido editado en el Edit Buffer y el sonido guardado originalmente.\n"
        "• WRITE: Guarda el Edit Buffer actual en una ranura MEMORY (A-1 a D-8).\n"
        "  IMPORTANTE: El interruptor trasero MEMORY PROTECT debe estar en OFF antes de pulsar WRITE.\n"
        "• CARTUCHO RAM RA-3: Cartucho externo opcional con 32 voces adicionales (Bancos A-D).\n"
        "  - SAVE: Copia la Memoria Interna (A,B o C,D) al Cartucho.\n"
        "  - LOAD: Carga el Cartucho (A,B o C,D) en la Memoria Interna.\n"
        "  - Utiliza una pila de litio BR-2016.");

    add(Language::Es, "07_mix_split",
        "7. Modos Tone Mix y Key Split",
        "Capas de Sonido y División de Teclado (Manual pp. 28-31)",
        "MODO TONE MIX:\n"
        "• Superpone dos Timbres (Tone 1 + Tone 2) en todo el teclado.\n"
        "• Los niveles LEV1 (1-15) y LEV2 (1-15) ajustan la mezcla.\n"
        "• Polifonía: 4 notas si ambos Timbres usan 2 Líneas; hasta 8 notas si ambos usan 1 Línea.\n\n"
        "MODO KEY SPLIT:\n"
        "• Divide las 61 teclas en zonas LOWER y UPPER.\n"
        "• Pulsa SPLIT POINT y toca una tecla (1-60) para fijar el punto de división.\n"
        "• Niveles LOWER LEV (1-15), UPPER LEV (1-15), Portamento, Glide y Transpose independientes por zona.");

    add(Language::Es, "08_midi",
        "8. MIDI y Tabla de Implementación",
        "P.MODE, M.MODE, Multitímbrico y SysEx (Manual pp. 32-39)",
        "MODOS MIDI:\n"
        "• P.MODE (Performance Mode): Operación MIDI estándar. Configura KBCH (canal Tx/Rx) y PRG=ENA/DIS.\n"
        "• M.MODE (Multi-Timbral Mode): Convierte el CZ-3000 en un módulo multitímbrico de 4 canales asignando timbres independientes a canales MIDI consecutivos.\n\n"
        "MAPEO DE PROGRAM CHANGE:\n"
        "• Programa 0-31 -> PRESET interno A-1 a D-8\n"
        "• Programa 32-63 -> MEMORY interna A-1 a D-8\n"
        "• Programa 64-95 -> CARTRIDGE A-1 a D-8\n\n"
        "FORMATO SYSEX:\n"
        "• ID de fabricante: 44H (Casio). Cada patch ocupa 263/264 bytes con parámetros en nibbles de 4 bits (nibble bajo primero).");

    add(Language::Es, "09_specs",
        "9. Cuidados y Especificaciones",
        "Mantenimiento y Ficha Técnica (Manual pp. 40-42)",
        "CUIDADOS Y PRECAUCIONES:\n"
        "1. Evitar calor extremo, luz solar directa, humedad o polvo.\n"
        "2. No golpear ni dejar caer el instrumento.\n"
        "3. Nunca usar bencina, disolventes o limpiadores químicos.\n"
        "4. Cambiar las 3 pilas AA de respaldo cada 1-2 años con el equipo ENCENDIDO para no perder las memorias RAM.\n"
        "5. Verificar el selector de voltaje trasero (100V/120V/220V/240V) antes de conectar.\n"
        "6. Mantener MEMORY PROTECT en ON durante actuaciones en vivo.\n\n"
        "ESPECIFICACIONES:\n"
        "• Teclado: 61 teclas estándar (C1-C6)\n"
        "• Fuente Sonora: Sistema PD (Phase Distortion)\n"
        "• Polifonía: 16 voces (1 Línea) / 8 voces (2 Líneas)\n"
        "• Memoria: 32 Preset + 32 Memoria Interna + 32 Cartucho RA-3\n"
        "• Conexiones: LINE OUT L/R, PHONES, SUSTAIN, PORTAMENTO, MIDI IN/OUT/THRU\n"
        "• Alimentación: CA 100/120/220/240V, 26W; Respaldo: 3 pilas AA\n"
        "• Dimensiones y Peso: 1025 x 341 x 125 mm, 11.7 kg");

    // =========================================================================
    // 3. ITALIAN (It)
    // =========================================================================
    add(Language::It, "01_general",
        "1. Panoramica e Guida Generale",
        "3 Modalità Operative e Pannello (Manuale pp. 2-5)",
        "Il Casio CZ-3000 è un sintetizzatore digitale a Distorsione di Fase (PD) a 61 tasti e 16 voci di polifonia.\n\n"
        "TRE MODALITÀ OPERATIVE:\n"
        "• NORMAL: Suona un singolo timbro su tutta la tastiera (16 voci a 1 Linea, 8 voci a 2 Linee).\n"
        "• TONE MIX: Sovrappone due timbri (polifonia 4 note; 8 note se entrambi a 1 Linea) con bilanciamento LEV1/LEV2.\n"
        "• KEY SPLIT: Divide la tastiera in zone LOWER e UPPER tramite Split Point (tasti 1-60) con effetti indipendenti.\n\n"
        "SEZIONI DEL PANNELLO (1-14): TOTAL CONTROL, MODE SELECT, SOLO, MIDI, PROGRAMMER, LCD 16x2, DATA ENTRY, CARTRIDGE RA-3, EFFECT, PARAMETER, Ruote Pitch Bend/Modulation, Pannello Posteriore (LINE OUT L/R, PHONES, SUSTAIN, PORTAMENTO, MIDI IN/OUT/THRU) e interruttore MEMORY PROTECT.");

    add(Language::It, "02_presets",
        "2. Modo Normal: Preset e Memory",
        "32 Preset Interni e 32 Memorie Utente (Manuale pp. 6-8)",
        "In modalità NORMAL seleziona Bank (A-D) e Number (1-8).\n"
        "• PRESET (32 voci): Suoni ROM di fabbrica di sola lettura (A-1..D-8).\n"
        "• MEMORY (32 voci): Memorie RAM programmabili (A-1..D-8) mantenute da 3 batterie AA (il display mostra *MEMORY* A-1).\n\n"
        "Nella tabella seguente, le voci con (*) usano 1 Linea DCO (16 voci); le altre usano 2 Linee (8 voci):");

    add(Language::It, "03_effects",
        "3. Effetti e Controllo Globale",
        "Portamento, Glide, Bend, Mod, Tune, Transpose (Manuale pp. 9-10)",
        "• PORTAMENTO (ON/OFF, TIME 00-99): Glissato fluido tra una nota e la successiva.\n"
        "• GLIDE (ON/OFF, NOTE -24..+24, TIME 00-99): Scorrimento fisso all'attacco di ogni tasto.\n"
        "• BEND RANGE (00-12 semitoni), MODULATION (DEPTH 00-99), MASTER TUNE (±100 cent / La4=442Hz), KEY TRANSPOSE (G-F#), SOLO (monofonico).");

    add(Language::It, "04_pd_env",
        "4. Sorgente PD e Inviluppi a 8 Stadi",
        "Principio Phase Distortion (Manuale pp. 11-15)",
        "La sintesi PD legge un'onda coseno digitale distorcendo la velocità dell'angolo di fase: DCO (pitch), DCW (brillantezza/armoniche) e DCA (ampiezza).\n\n"
        "Ogni blocco dispone di un inviluppo a 8 stadi con RATE (00-99), LEVEL (00-99), punto SUSTAIN (1-8 o OFF) e passo END (1-8).");

    add(Language::It, "05_params",
        "5. Parametri e Initialize",
        "Forme d'Onda, Line Select, Detune, Vibrato, Key Follow (Manuale pp. 16-24)",
        "33 forme d'onda combinando W1+W2 tra 8 forme base (1:Saw, 2:Square, 3:Pulse, 4:Double Sine, 5:Saw-Pulse, 6-8:Resonance I-III).\n"
        "KEY FOLLOW (0-9) per DCW/DCA; VIBRATO (Wave 1-4, Delay, Rate, Depth); LINE SELECT (1, 2, 1+2', 1+1'); DETUNE (+/- Oct, Note, Fine 0-60); RING MOD & NOISE MOD.\n\n"
        "INITIALIZE ripristina il buffer a una sinusoide neutra:");

    add(Language::It, "06_storage",
        "6. Write, Compare e Cartuccia RA-3",
        "Salvataggio e Cartuccia RAM (Manuale pp. 25-27)",
        "• COMPARE/RECALL: Confronta l'Edit Buffer con il suono salvato.\n"
        "• WRITE: Salva in MEMORY (A-1..D-8) con MEMORY PROTECT su OFF.\n"
        "• RA-3 RAM CARTRIDGE: 32 voci extra con comandi SAVE/LOAD (A,B o C,D) e batteria al litio BR-2016.");

    add(Language::It, "07_mix_split",
        "7. Modi Tone Mix e Key Split",
        "Layering e Divisione Tastiera (Manuale pp. 28-31)",
        "• TONE MIX: Sovrappone Tone 1 + Tone 2 (LEV1/LEV2 1-15; polifonia 4 note con 2 Linee, 8 note con 1 Linea).\n"
        "• KEY SPLIT: Divide i 61 tasti in LOWER e UPPER allo SPLIT POINT (1-60) con livelli ed effetti indipendenti.");

    add(Language::It, "08_midi",
        "8. MIDI e Tabella di Implementazione",
        "P.MODE, M.MODE, Multitimbrico e SysEx (Manuale pp. 32-39)",
        "• P.MODE: Uso MIDI standard (KBCH, PRG=ENA/DIS).\n"
        "• M.MODE: Espansore multitimbrico a 4 canali su canali MIDI consecutivi.\n"
        "• Program Change: 0-31 Preset, 32-63 Memory, 64-95 Cartridge.\n"
        "• SysEx Casio (ID 44H): Patch da 263/264 byte codificati in nibble da 4 bit.");

    add(Language::It, "09_specs",
        "9. Cura e Specifiche Tecniche",
        "Manutenzione e Dati Tecnici (Manuale pp. 40-42)",
        "Sostituire le 3 batterie AA di backup ogni 1-2 anni a strumento ACCESO. Verificare il selettore tensione (100/120/220/240V).\n"
        "Specifiche: 61 tasti, sorgente PD, 16/8 voci, 32 Preset + 32 Memory + 32 Cartridge, 26W, 1025x341x125 mm, 11.7 kg.");

    // =========================================================================
    // 4. GERMAN (De)
    // =========================================================================
    add(Language::De, "01_general",
        "1. Übersicht & Bedienfeld",
        "3 Betriebsarten & Aufbau (Handbuch S. 2-5)",
        "Der Casio CZ-3000 ist ein 61-Tasten, 16-stimmig polyphoner Phase-Distortion-Synthesizer (PD).\n\n"
        "DREI BETRIEBSARTEN:\n"
        "• NORMAL: Ein Klang über die gesamte Tastatur (16 Stimmen bei 1 Line, 8 Stimmen bei 2 Lines).\n"
        "• TONE MIX: Schichtet zwei Klänge (4-stimmig; 8-stimmig bei je 1 Line) mit LEV1/LEV2.\n"
        "• KEY SPLIT: Teilt die Tastatur am Split Point (1-60) in LOWER und UPPER mit eigenen Effekten.\n\n"
        "BEDIENFELD (1-14): TOTAL CONTROL, MODE SELECT, SOLO, MIDI, PROGRAMMER, 16x2 LCD, DATA ENTRY, CARTRIDGE RA-3, EFFECT, PARAMETER, Pitch-Bend/Mod-Räder, Rückseite (LINE OUT L/R, PHONES, SUSTAIN, PORTAMENTO, MIDI IN/OUT/THRU) & MEMORY PROTECT.");

    add(Language::De, "02_presets",
        "2. Normal-Modus: Preset & Memory",
        "32 Werks-Presets & 32 User-Speicher (Handbuch S. 6-8)",
        "Wählen Sie im NORMAL-Modus Bank (A-D) und Nummer (1-8).\n"
        "• PRESET (32): Feste ROM-Werksklänge (A-1..D-8).\n"
        "• MEMORY (32): Programmierbare RAM-Plätze (A-1..D-8), gesichert durch 3x AA-Batterien (*MEMORY* A-1).\n\n"
        "Klänge mit (*) nutzen 1 DCO-Line (16 Stimmen), alle anderen 2 Lines (8 Stimmen):");

    add(Language::De, "03_effects",
        "3. Effekte & Globalsteuerung",
        "Portamento, Glide, Bend, Mod, Tune, Transpose (Handbuch S. 9-10)",
        "• PORTAMENTO (ON/OFF, TIME 00-99), GLIDE (ON/OFF, NOTE -24..+24, TIME 00-99), BEND RANGE (00-12), MODULATION (DEPTH 00-99), MASTER TUNE (±100 Cent / A4=442Hz), KEY TRANSPOSE (G-F#), SOLO (monophon).");

    add(Language::De, "04_pd_env",
        "4. PD-Klangerzeugung & Hüllkurven",
        "Phase Distortion & 8-Stufen-Hüllkurven (Handbuch S. 11-15)",
        "PD verzerrt die Auslesegeschwindigkeit einer digitalen Kosinuswelle: DCO (Tonhöhe), DCW (Obertongehalt/Helligkeit), DCA (Lautstärke).\n\n"
        "Jeder Block besitzt eine 8-Stufen-Hüllkurve mit RATE (00-99), LEVEL (00-99), SUSTAIN (1-8/OFF) und END (1-8).");

    add(Language::De, "05_params",
        "5. Klangparameter & Initialize",
        "Wellenformen, Line Select, Detune, Vibrato, Key Follow (Handbuch S. 16-24)",
        "33 Wellenformen aus 8 Grundformen (1:Saw, 2:Square, 3:Pulse, 4:Double Sine, 5:Saw-Pulse, 6-8:Resonance I-III).\n"
        "KEY FOLLOW (0-9), VIBRATO (Wave 1-4, Delay, Rate, Depth), LINE SELECT (1, 2, 1+2', 1+1'), DETUNE (+/- Oct, Note, Fine 0-60), RING MOD & NOISE MOD.\n\n"
        "INITIALIZE setzt den Edit-Buffer auf einen neutralen Sinus:");

    add(Language::De, "06_storage",
        "6. Write, Compare & RA-3 Cartridge",
        "Speichern & RAM-Cartridge (Handbuch S. 25-27)",
        "• COMPARE/RECALL: Vergleicht Edit-Buffer mit dem Original.\n"
        "• WRITE: Speichert in MEMORY (A-1..D-8) bei MEMORY PROTECT = OFF.\n"
        "• RA-3 RAM CARTRIDGE: 32 Extraspeicher (SAVE/LOAD A,B oder C,D; BR-2016 Lithium-Batterie).");

    add(Language::De, "07_mix_split",
        "7. Tone Mix & Key Split",
        "Layering & Tastaturteilung (Handbuch S. 28-31)",
        "• TONE MIX: Kombiniert Tone 1 + Tone 2 (LEV1/LEV2 1-15; 4 Stimmen bzw. 8 bei 1-Line).\n"
        "• KEY SPLIT: Teilt 61 Tasten am SPLIT POINT (1-60) in LOWER und UPPER.");

    add(Language::De, "08_midi",
        "8. MIDI & Implementierungstabelle",
        "P.MODE, M.MODE, Multi-Timbral & SysEx (Handbuch S. 32-39)",
        "• P.MODE: Standard-MIDI (KBCH, PRG=ENA/DIS).\n"
        "• M.MODE: 4-kanaliger Multi-Timbral-Expander.\n"
        "• Program Change: 0-31 Preset, 32-63 Memory, 64-95 Cartridge.\n"
        "• SysEx (ID 44H): 263/264-Byte Patch-Daten im 4-Bit-Nibble-Format.");

    add(Language::De, "09_specs",
        "9. Pflege & Technische Daten",
        "Wartung & Spezifikationen (Handbuch S. 40-42)",
        "3x AA Speicherbatterien alle 1-2 Jahre im eingeschalteten Zustand wechseln. Spannungswahlschalter (100/120/220/240V) beachten.\n"
        "Daten: 61 Tasten, PD-Quelle, 16/8 Stimmen, 26W, 1025x341x125 mm, 11.7 kg.");

    // =========================================================================
    // 5. JAPANESE (Ja)
    // =========================================================================
    add(Language::Ja, "01_general",
        "1. 概要と各部の名称",
        "3つの演奏モードとパネル構成 (取扱説明書 pp. 2-5)",
        "Casio CZ-3000 は 61鍵・最大16音ポリフォニックのフェイズ・ディストーション (PD) デジタルシンセサイザーです。\n\n"
        "3つの演奏モード:\n"
        "• NORMAL: 鍵盤全体で1音色を演奏（1ライン時16音、2ライン時8音ポリ）。\n"
        "• TONE MIX: 2音色を重ねて演奏（4音ポリ、双方1ライン時は8音ポリ）。LEV1/LEV2でバランス調整。\n"
        "• KEY SPLIT: スプリットポイント (1-60) でLOWERとUPPERに分割し、各々独立したエフェクトを設定可能。\n\n"
        "パネル各部 (1-14): TOTAL CONTROL, MODE SELECT, SOLO, MIDI, PROGRAMMER, 16x2 LCD, DATA ENTRY, CARTRIDGE (RA-3), EFFECT, PARAMETER, ベンド/モジュレーションホイール, リアパネル (LINE OUT L/R, PHONES, SUSTAIN, PORTAMENTO, MIDI IN/OUT/THRU), MEMORY PROTECT スイッチ。");

    add(Language::Ja, "02_presets",
        "2. ノーマルモード: プリセット & メモリー",
        "32プリセット & 32ユーザーメモリー (pp. 6-8)",
        "Bank (A-D) と Number (1-8) で音色を選びます。\n"
        "• PRESET (32音色): 読み取り専用の工場出荷ROM音色 (A-1〜D-8)。\n"
        "• MEMORY (32音色): 単3乾電池3本でバックアップされるユーザーRAM音色 (実機LCDには *MEMORY* A-1 と表示)。\n\n"
        "下表で (*) 付きは1ラインDCO (16音ポリ)、その他は2ライン (8音ポリ) です:");

    add(Language::Ja, "03_effects",
        "3. エフェクト & 全体コントロール",
        "ポルタメント・グライド・ベンド・チューン (pp. 9-10)",
        "• PORTAMENTO (ON/OFF, TIME 00-99): 音程を滑らかに移行。\n"
        "• GLIDE (ON/OFF, NOTE -24〜+24, TIME 00-99): 押鍵ごとに一定音程からスウィープ。\n"
        "• BEND RANGE (00-12), MODULATION (DEPTH 00-99), MASTER TUNE (±100セント / A4=442Hz), KEY TRANSPOSE (G〜F#), SOLO (モノフォニック)。");

    add(Language::Ja, "04_pd_env",
        "4. PD音源と8ステップエンベロープ",
        "位相歪み原理とエンベロープ (pp. 11-15)",
        "PD音源はメモリ内の余弦波の読み出し速度(位相角)を歪ませて倍音を生成します: DCO (音程), DCW (明るさ/倍音), DCA (音量)。\n\n"
        "各ブロックに8段階エンベロープを搭載: RATE (00-99), LEVEL (00-99), SUSTAIN (1-8/OFF), END (1-8)。");

    add(Language::Ja, "05_params",
        "5. サウンドパラメーター & イニシャライズ",
        "波形・ラインセレクト・デチューン・ビブラート (pp. 16-24)",
        "8種の基本波形 (1:Saw, 2:Square, 3:Pulse, 4:Double Sine, 5:Saw-Pulse, 6-8:Resonance I-III) の組み合わせで33波形を生成。\n"
        "KEY FOLLOW (0-9), VIBRATO (Wave 1-4, Delay, Rate, Depth), LINE SELECT (1, 2, 1+2', 1+1'), DETUNE (+/- Oct, Note, Fine 0-60), RING MOD & NOISE MOD。\n\n"
        "INITIALIZE はエディットバッファを無色なサイン波へ初期化します:");

    add(Language::Ja, "06_storage",
        "6. 書込・比較 & RA-3 カートリッジ",
        "音色の保存とRAMカートリッジ (pp. 25-27)",
        "• COMPARE/RECALL: 編集中の音と元音色を比較。\n"
        "• WRITE: 背面の MEMORY PROTECT を OFF にして MEMORY (A-1〜D-8) へ保存。\n"
        "• RA-3 RAMカートリッジ: 32音色を追加保存 (SAVE/LOAD A,B または C,D、リチウム電池 BR-2016 使用)。");

    add(Language::Ja, "07_mix_split",
        "7. トーンミックス & キースプリット",
        "レイヤーと鍵盤分割 (pp. 28-31)",
        "• TONE MIX: Tone 1 + Tone 2 を重ねて演奏 (LEV1/LEV2 1-15、通常4音ポリ/1ライン時8音ポリ)。\n"
        "• KEY SPLIT: SPLIT POINT (1-60) でLOWERとUPPERに分割し、独立したレベルとエフェクトを設定。");

    add(Language::Ja, "08_midi",
        "8. MIDI とインプリメンテーション表",
        "P.MODE / M.MODE / マルチティンバー / SysEx (pp. 32-39)",
        "• P.MODE: 通常MIDIモード (KBCH, PRG=ENA/DIS)。\n"
        "• M.MODE: 連続したMIDIチャンネルで最大4パートのマルチティンバー音源として動作。\n"
        "• プログラムチェンジ: 0-31 Preset, 32-63 Memory, 64-95 Cartridge。\n"
        "• SysEx (ID 44H): 4ビットニブル符号化の263/264バイトデータ。");

    add(Language::Ja, "09_specs",
        "9. お手入れと主な仕様",
        "保守とスペック (pp. 40-42)",
        "バックアップ用単3乾電池3本は1〜2年ごとに電源ONの状態で交換してください。電圧切換スイッチ (100/120/220/240V) に注意。\n"
        "仕様: 61鍵, PD音源, 16/8音ポリ, 26W, 1025x341x125 mm, 11.7 kg。");

    // =========================================================================
    // 6. SIMPLIFIED CHINESE (Zh)
    // =========================================================================
    add(Language::Zh, "01_general",
        "1. 概述与面板指南",
        "3 种演奏模式与面板布局 (手册 pp. 2-5)",
        "Casio CZ-3000 是一款 61 键、16 复音相位失真 (PD) 数字合成器。\n\n"
        "三种演奏模式：\n"
        "• NORMAL：全键盘演奏单一音色（单线 16 复音，双线 8 复音）。\n"
        "• TONE MIX：双音色叠加（4 复音；若均为单线则 8 复音），独立调节 LEV1/LEV2。\n"
        "• KEY SPLIT：在分割点 (1-60) 将键盘分为 LOWER 和 UPPER 两区，各自拥有独立效果。\n\n"
        "面板区域 (1-14)：总控、模式选择、SOLO、MIDI、编程器、16x2 LCD、数据输入、RA-3 存储卡槽、效果、参数、弯音/调制轮、背板接口与 MEMORY PROTECT 开关。");

    add(Language::Zh, "02_presets",
        "2. Normal 模式：预设与用户记忆",
        "32 个内置预设与 32 个用户记忆 (pp. 6-8)",
        "在 NORMAL 模式下通过 Bank (A-D) 与 Number (1-8) 选择音色。\n"
        "• PRESET (32)：只读出厂 ROM 音色 (A-1..D-8)。\n"
        "• MEMORY (32)：可编程 RAM 槽位，由 3 节 AA 电池供电保存（硬件 LCD 显示 *MEMORY* A-1）。\n\n"
        "下表中带 (*) 的音色使用单线 DCO（16 复音），其余使用双线（8 复音）：");

    add(Language::Zh, "03_effects",
        "3. 效果与全局控制",
        "滑音、弯音、调制、调音、移调 (pp. 9-10)",
        "• PORTAMENTO (ON/OFF, TIME 00-99)、GLIDE (ON/OFF, NOTE -24..+24, TIME 00-99)、BEND RANGE (00-12 半音)、MODULATION (DEPTH 00-99)、MASTER TUNE (±100 音分 / A4=442Hz)、KEY TRANSPOSE (G-F#)、SOLO (单音模式)。");

    add(Language::Zh, "04_pd_env",
        "4. PD 音源与 8 段包络",
        "相位失真原理与包络 (pp. 11-15)",
        "PD 通过改变读取数字余弦波相位角的速度来生成谐波：DCO（音高）、DCW（亮度/谐波）、DCA（音量）。\n\n"
        "每个模块拥有独立 8 段包络：RATE (00-99)、LEVEL (00-99)、SUSTAIN (1-8/OFF) 与 END (1-8)。");

    add(Language::Zh, "05_params",
        "5. 音色参数与初始化",
        "波形、线路选择、失谐、颤音、键盘跟踪 (pp. 16-24)",
        "8 种基础波形组合出 33 种频谱（1:Saw, 2:Square, 3:Pulse, 4:Double Sine, 5:Saw-Pulse, 6-8:Resonance I-III）。\n"
        "KEY FOLLOW (0-9)、VIBRATO (Wave 1-4, Delay, Rate, Depth)、LINE SELECT (1, 2, 1+2', 1+1')、DETUNE (+/- Oct, Note, Fine 0-60)、RING MOD 与 NOISE MOD。\n\n"
        "INITIALIZE 将编辑缓冲区重置为中性正弦波：");

    add(Language::Zh, "06_storage",
        "6. 写入、对比与 RA-3 存储卡",
        "保存音色与 RAM 卡 (pp. 25-27)",
        "• COMPARE/RECALL：对比编辑前后的音色。\n"
        "• WRITE：将背板 MEMORY PROTECT 拨至 OFF 后写入 MEMORY (A-1..D-8)。\n"
        "• RA-3 RAM 卡：额外存储 32 个音色（SAVE/LOAD A,B 或 C,D，使用 BR-2016 锂电池）。");

    add(Language::Zh, "07_mix_split",
        "7. Tone Mix 与 Key Split 模式",
        "分层与键盘分割 (pp. 28-31)",
        "• TONE MIX：叠加 Tone 1 + Tone 2（LEV1/LEV2 1-15；4 复音或单线时 8 复音）。\n"
        "• KEY SPLIT：在 SPLIT POINT (1-60) 处分割为 LOWER 与 UPPER。");

    add(Language::Zh, "08_midi",
        "8. MIDI 与实现图表",
        "P.MODE、M.MODE、多音色与 SysEx (pp. 32-39)",
        "• P.MODE：标准 MIDI 模式 (KBCH, PRG=ENA/DIS)。\n"
        "• M.MODE：4 通道多音色扩展器模式。\n"
        "• Program Change：0-31 Preset，32-63 Memory，64-95 Cartridge。\n"
        "• SysEx (ID 44H)：263/264 字节，4 位半字节编码。");

    add(Language::Zh, "09_specs",
        "9. 保养与技术规格",
        "维护与规格参数 (pp. 40-42)",
        "请在开机状态下每 1-2 年更换 3 节 AA 备份电池；通电前确认电压选择器 (100/120/220/240V)。\n"
        "规格：61 键，PD 音源，16/8 复音，26W，1025x341x125 mm，11.7 kg。");

    // =========================================================================
    // 7. BRAZILIAN PORTUGUESE (PtBr)
    // =========================================================================
    add(Language::PtBr, "01_general",
        "1. Visão Geral e Painel",
        "3 Modos de Operação e Layout (Manual pp. 2-5)",
        "O Casio CZ-3000 é um sintetizador digital de Distorção de Fase (PD) com 61 teclas e 16 vozes de polifonia.\n\n"
        "TRÊS MODOS DE OPERAÇÃO:\n"
        "• NORMAL: Toca um único timbre em todo o teclado (16 vozes com 1 Linha, 8 vozes com 2 Linhas).\n"
        "• TONE MIX: Sobrepõe dois timbres (4 vozes; 8 vozes se ambos usarem 1 Linha) com balanço LEV1/LEV2.\n"
        "• KEY SPLIT: Divide o teclado em LOWER e UPPER no Split Point (1-60) com efeitos independentes.\n\n"
        "SEÇÕES DO PAINEL (1-14): TOTAL CONTROL, MODE SELECT, SOLO, MIDI, PROGRAMMER, LCD 16x2, DATA ENTRY, CARTRIDGE RA-3, EFFECT, PARAMETER, Rodas Pitch Bend/Modulation, Painel Traseiro e chave MEMORY PROTECT.");

    add(Language::PtBr, "02_presets",
        "2. Modo Normal: Preset e Memory",
        "32 Presets de Fábrica e 32 Memórias (pp. 6-8)",
        "Selecione Banco (A-D) e Número (1-8).\n"
        "• PRESET (32): Sons ROM de fábrica somente leitura.\n"
        "• MEMORY (32): Slots RAM programáveis com backup de 3 pilhas AA (no visor aparece *MEMORY* A-1).\n\n"
        "Timbres marcados com (*) usam 1 Linha DCO (16 vozes); os demais usam 2 Linhas (8 vozes):");

    add(Language::PtBr, "03_effects",
        "3. Efeitos e Controle Global",
        "Portamento, Glide, Bend, Mod, Tune, Transpose (pp. 9-10)",
        "• PORTAMENTO (ON/OFF, TIME 00-99), GLIDE (ON/OFF, NOTE -24..+24, TIME 00-99), BEND RANGE (00-12), MODULATION (DEPTH 00-99), MASTER TUNE (±100 cents / A4=442Hz), KEY TRANSPOSE (G-F#), SOLO (monofônico).");

    add(Language::PtBr, "04_pd_env",
        "4. Fonte PD e Envelopes de 8 Passos",
        "Distorção de Fase e Envelopes (pp. 11-15)",
        "A síntese PD lê uma onda cosseno distorcendo a velocidade do ângulo de fase: DCO (afinação), DCW (brilho/harmônicos) e DCA (volume).\n\n"
        "Cada bloco possui envelope de 8 passos com RATE (00-99), LEVEL (00-99), SUSTAIN (1-8/OFF) e END (1-8).");

    add(Language::PtBr, "05_params",
        "5. Parâmetros e Initialize",
        "Formas de Onda, Line Select, Detune, Vibrato, Key Follow (pp. 16-24)",
        "33 formas de onda combinando W1+W2 entre 8 ondas base (1:Saw, 2:Square, 3:Pulse, 4:Double Sine, 5:Saw-Pulse, 6-8:Resonance I-III).\n"
        "KEY FOLLOW (0-9), VIBRATO (Wave 1-4, Delay, Rate, Depth), LINE SELECT (1, 2, 1+2', 1+1'), DETUNE (+/- Oct, Note, Fine 0-60), RING MOD e NOISE MOD.\n\n"
        "INITIALIZE reinicia o Edit Buffer para uma senoide neutra:");

    add(Language::PtBr, "06_storage",
        "6. Write, Compare e Cartucho RA-3",
        "Gravação e Cartucho RAM (pp. 25-27)",
        "• COMPARE/RECALL: Compara o Edit Buffer com o som salvo.\n"
        "• WRITE: Salva em MEMORY (A-1..D-8) com MEMORY PROTECT em OFF.\n"
        "• CARTUCHO RA-3: 32 vozes extras (SAVE/LOAD A,B ou C,D; bateria BR-2016).");

    add(Language::PtBr, "07_mix_split",
        "7. Modos Tone Mix e Key Split",
        "Camadas e Divisão de Teclado (pp. 28-31)",
        "• TONE MIX: Sobrepõe Tone 1 + Tone 2 (LEV1/LEV2 1-15; 4 vozes ou 8 com 1 Linha).\n"
        "• KEY SPLIT: Divide as 61 teclas em LOWER e UPPER no SPLIT POINT (1-60).");

    add(Language::PtBr, "08_midi",
        "8. MIDI e Tabela de Implementação",
        "P.MODE, M.MODE, Multitimbral e SysEx (pp. 32-39)",
        "• P.MODE: Operação MIDI padrão (KBCH, PRG=ENA/DIS).\n"
        "• M.MODE: Expansor multitimbral de 4 canais.\n"
        "• Program Change: 0-31 Preset, 32-63 Memory, 64-95 Cartridge.\n"
        "• SysEx (ID 44H): 263/264 bytes em nibbles de 4 bits.");

    add(Language::PtBr, "09_specs",
        "9. Cuidados e Especificações",
        "Manutenção e Ficha Técnica (pp. 40-42)",
        "Troque as 3 pilhas AA de backup a cada 1-2 anos com o aparelho LIGADO. Verifique o seletor de voltagem (100/120/220/240V).\n"
        "Especificações: 61 teclas, fonte PD, 16/8 vozes, 26W, 1025x341x125 mm, 11.7 kg.");

    // =========================================================================
    // 8. FRENCH (Fr)
    // =========================================================================
    add(Language::Fr, "01_general",
        "1. Vue d'ensemble et Panneau",
        "3 Modes de Jeu et Façades (Manuel pp. 2-5)",
        "Le Casio CZ-3000 est un synthétiseur numérique à Distorsion de Phase (PD) de 61 touches et 16 voix de polyphonie.\n\n"
        "TROIS MODES DE JEU :\n"
        "• NORMAL : Un timbre sur tout le clavier (16 voix à 1 Ligne, 8 voix à 2 Lignes).\n"
        "• TONE MIX : Superpose deux timbres (4 voix ; 8 voix si 1 Ligne chacun) avec balance LEV1/LEV2.\n"
        "• KEY SPLIT : Divise le clavier en zones LOWER et UPPER au Split Point (1-60) avec effets indépendants.\n\n"
        "SECTIONS DU PANNEAU (1-14) : TOTAL CONTROL, MODE SELECT, SOLO, MIDI, PROGRAMMER, LCD 16x2, DATA ENTRY, CARTRIDGE RA-3, EFFECT, PARAMETER, Molettes Pitch Bend/Modulation, Face arrière et interrupteur MEMORY PROTECT.");

    add(Language::Fr, "02_presets",
        "2. Mode Normal : Preset et Memory",
        "32 Presets d'usine et 32 Mémoires (pp. 6-8)",
        "Sélectionnez Banque (A-D) et Numéro (1-8).\n"
        "• PRESET (32) : Sons ROM d'usine en lecture seule.\n"
        "• MEMORY (32) : Emplacements RAM programmables sauvegardés par 3 piles AA (affiche *MEMORY* A-1).\n\n"
        "Les timbres marqués (*) utilisent 1 Ligne DCO (16 voix), les autres 2 Lignes (8 voix) :");

    add(Language::Fr, "03_effects",
        "3. Effets et Contrôle Global",
        "Portamento, Glide, Bend, Mod, Tune, Transpose (pp. 9-10)",
        "• PORTAMENTO (ON/OFF, TIME 00-99), GLIDE (ON/OFF, NOTE -24..+24, TIME 00-99), BEND RANGE (00-12), MODULATION (DEPTH 00-99), MASTER TUNE (±100 cents / A4=442Hz), KEY TRANSPOSE (G-F#), SOLO (monophonique).");

    add(Language::Fr, "04_pd_env",
        "4. Source PD et Enveloppes à 8 Étages",
        "Principe Phase Distortion (pp. 11-15)",
        "La synthèse PD lit une onde cosinus en distordant la vitesse de son angle de phase : DCO (hauteur), DCW (brillance/harmoniques) et DCA (volume).\n\n"
        "Chaque bloc dispose d'une enveloppe à 8 étapes avec RATE (00-99), LEVEL (00-99), SUSTAIN (1-8/OFF) et END (1-8).");

    add(Language::Fr, "05_params",
        "5. Paramètres et Initialize",
        "Ondes, Line Select, Detune, Vibrato, Key Follow (pp. 16-24)",
        "33 formes d'onde en combinant W1+W2 parmi 8 formes de base (1:Saw, 2:Square, 3:Pulse, 4:Double Sine, 5:Saw-Pulse, 6-8:Resonance I-III).\n"
        "KEY FOLLOW (0-9), VIBRATO (Wave 1-4, Delay, Rate, Depth), LINE SELECT (1, 2, 1+2', 1+1'), DETUNE (+/- Oct, Note, Fine 0-60), RING MOD & NOISE MOD.\n\n"
        "INITIALIZE réinitialise le tampon d'édition sur une sinusoïde neutre :");

    add(Language::Fr, "06_storage",
        "6. Write, Compare et Cartouche RA-3",
        "Sauvegarde et Cartouche RAM (pp. 25-27)",
        "• COMPARE/RECALL : Compare le tampon d'édition au son d'origine.\n"
        "• WRITE : Enregistre dans MEMORY (A-1..D-8) avec MEMORY PROTECT sur OFF.\n"
        "• CARTOUCHE RA-3 : 32 sons supplémentaires (SAVE/LOAD A,B ou C,D ; pile BR-2016).");

    add(Language::Fr, "07_mix_split",
        "7. Modes Tone Mix et Key Split",
        "Superposition et Partage du Clavier (pp. 28-31)",
        "• TONE MIX : Superpose Tone 1 + Tone 2 (LEV1/LEV2 1-15 ; 4 voix ou 8 en 1 Ligne).\n"
        "• KEY SPLIT : Divise les 61 touches en LOWER et UPPER au SPLIT POINT (1-60).");

    add(Language::Fr, "08_midi",
        "8. MIDI et Tableau d'Implémentation",
        "P.MODE, M.MODE, Multitimbral et SysEx (pp. 32-39)",
        "• P.MODE : Mode MIDI standard (KBCH, PRG=ENA/DIS).\n"
        "• M.MODE : Expandeur multitimbral à 4 canaux.\n"
        "• Program Change : 0-31 Preset, 32-63 Memory, 64-95 Cartridge.\n"
        "• SysEx (ID 44H) : 263/264 octets encodés en nibbles de 4 bits.");

    add(Language::Fr, "09_specs",
        "9. Entretien et Spécifications",
        "Maintenance et Fiche Technique (pp. 40-42)",
        "Remplacez les 3 piles AA de sauvegarde tous les 1-2 ans appareil ALLUMÉ. Vérifiez le sélecteur de tension (100/120/220/240V).\n"
        "Spécifications : 61 touches, source PD, 16/8 voix, 26W, 1025x341x125 mm, 11,7 kg.");
}

void CzManual::render_preset_table() {
    static const char* rows[8][4] = {
        { "A-1  Brass Ens 1",        "B-1  String Ens 1",       "C-1  Trumpet",            "D-1  Elec Organ 1 (*)" },
        { "A-2  Brass Ens 2",        "B-2  String Ens 2",       "C-2  Trombone (*)",       "D-2  Pipe Organ 1" },
        { "A-3  Accordion",          "B-3  Elec Piano 1",       "C-3  Flute (*)",          "D-3  Slap Bass (*)" },
        { "A-4  Harpsichord 1 (*)",  "B-4  Elec Piano 2",       "C-4  Clarinet (*)",       "D-4  Elec Bass (*)" },
        { "A-5  Vibraphone 1 (*)",   "B-5  Clavinet (*)",       "C-5  Oboe (*)",           "D-5  Wood Bass (*)" },
        { "A-6  Marimba (*)",        "B-6  Honky-Tonk Piano",   "C-6  Saxophone (*)",      "D-6  Synth Bass (*)" },
        { "A-7  Acoustic Guitar (*)","B-7  Koto (*)",           "C-7  Violin (*)",         "D-7  Fantasy 1" },
        { "A-8  Elec Guitar (*)",    "B-8  Harp (*)",           "C-8  Cello (*)",          "D-8  Cosmic Sound" }
    };

    if (ImGui::BeginTable("PresetTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableSetupColumn("Bank A");
        ImGui::TableSetupColumn("Bank B");
        ImGui::TableSetupColumn("Bank C");
        ImGui::TableSetupColumn("Bank D");
        ImGui::TableHeadersRow();
        for (int r = 0; r < 8; ++r) {
            ImGui::TableNextRow();
            for (int c = 0; c < 4; ++c) {
                ImGui::TableSetColumnIndex(c);
                ImGui::TextUnformatted(rows[r][c]);
            }
        }
        ImGui::EndTable();
    }
}

void CzManual::render_initialize_table() {
    static const char* init_rows[][2] = {
        { "LINE SELECT", "1" },
        { "DCO1 WAVE FORM", "1 (Sawtooth), W2=0" },
        { "DCO1 / DCW1 / DCA1 ENV", "STEP 1: RATE=99, LEVEL=99 (DCA), END=1" },
        { "DCW1 / DCA1 KEY FOLLOW", "0" },
        { "VIBRATO", "WAVE=1 (Triangle), DELAY=00, RATE=00, DEPTH=00" },
        { "OCTAVE", "0" },
        { "DETUNE", "+, OCT=0, NOTE=00, FINE=00" },
        { "RING / NOISE MOD", "OFF" }
    };

    if (ImGui::BeginTable("InitTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 210.0f);
        ImGui::TableSetupColumn("Initialized Value");
        ImGui::TableHeadersRow();
        for (const auto& row : init_rows) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(row[0]);
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(row[1]);
        }
        ImGui::EndTable();
    }
}

void CzManual::render_midi_chart() {
    static const char* midi_rows[][4] = {
        { "Basic Channel",      "1 - 16",          "1 - 16",          "Memorized" },
        { "Mode",               "Mode 3",          "Mode 3 / 4 (M.MODE)", "Poly / Mono" },
        { "Note Number",        "36 - 96 (C2-C7)", "0 - 127",         "Transpose +/-12" },
        { "Velocity",           "Fixed (64)",      "Ignored",         "No velocity sense" },
        { "Pitch Bender",       "Yes (0-12 st)",   "Yes",             "7-bit" },
        { "Modulation (CC 1)",  "Yes",             "Yes",             "Vibrato depth" },
        { "Portamento (CC 65)", "Yes",             "Yes",             "On/Off switch" },
        { "Sustain (CC 64)",    "Yes",             "Yes",             "Hold pedal" },
        { "Program Change",     "0 - 95",          "0 - 95",          "Preset/Mem/Cart" },
        { "System Exclusive",   "Yes (ID 44H)",    "Yes (ID 44H)",    "263/264 bytes" }
    };

    if (ImGui::BeginTable("MidiChart", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Function");
        ImGui::TableSetupColumn("Transmitted");
        ImGui::TableSetupColumn("Recognized");
        ImGui::TableSetupColumn("Remarks");
        ImGui::TableHeadersRow();
        for (const auto& row : midi_rows) {
            ImGui::TableNextRow();
            for (int c = 0; c < 4; ++c) {
                ImGui::TableSetColumnIndex(c);
                ImGui::TextUnformatted(row[c]);
            }
        }
        ImGui::EndTable();
    }
}

void CzManual::render_manual_window(bool* p_open) {
    ImGui::SetNextWindowSize(ImVec2(860, 560), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(tr("manual.win_title"), p_open)) {
        ImGui::End();
        return;
    }

    size_t idx = static_cast<size_t>(CzI18n::instance().current_language());
    if (idx >= 8) idx = 0;
    const auto& secs = sections_[idx];

    ImGui::BeginChild("ManualNav", ImVec2(250, 0), true);
    ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "%s", tr("manual.sections"));
    ImGui::Separator();

    for (const auto& s : secs) {
        bool selected = (active_section_id_ == s.id);
        if (ImGui::Selectable(s.title.c_str(), selected)) {
            active_section_id_ = s.id;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("ManualContent", ImVec2(0, 0), true);
    for (const auto& s : secs) {
        if (s.id != active_section_id_) continue;

        ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "%s", s.title.c_str());
        ImGui::TextDisabled("%s", s.subtitle.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("%s", s.body.c_str());
        ImGui::Spacing();
        ImGui::Spacing();

        if (s.id == "02_presets") {
            render_preset_table();
        } else if (s.id == "05_params") {
            render_initialize_table();
        } else if (s.id == "08_midi") {
            render_midi_chart();
        }
        break;
    }
    ImGui::EndChild();

    ImGui::End();
}

} // namespace cz
