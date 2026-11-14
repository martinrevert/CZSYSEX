#include "cz_tutorial.h"
#include "../core/cz_i18n.h"
#include <imgui.h>

namespace cz {

CzTutorial& CzTutorial::instance() {
    static CzTutorial inst;
    return inst;
}

CzTutorial::CzTutorial() {
    init_topics();
}

const std::unordered_map<std::string, TutorialTopic>& CzTutorial::current_topics() const {
    size_t idx = static_cast<size_t>(CzI18n::instance().current_language());
    if (idx >= 8) idx = 0;
    return topics_[idx];
}

void CzTutorial::init_topics() {
    topic_order_ = {
        "intro", "dco", "dcw", "dca",
        "envelope", "sustain", "end_step",
        "waves", "line_select", "detune",
        "vibrato", "key_follow", "ring_noise", "sysex_io"
    };

    auto add = [&](Language lang, const char* id, const char* title, const char* subtitle,
                   const char* summary, const char* effect, const char* pd_tech) {
        topics_[static_cast<size_t>(lang)][id] = {title, subtitle, summary, effect, pd_tech};
    };

    // =========================================================================
    // ENGLISH (En)
    // =========================================================================
    add(Language::En, "intro",
        "What is Phase Distortion (PD)?",
        "Casio's digital synthesis (1984-1985)",
        "Unlike traditional analog synths (VCO -> VCF -> VCA) or Yamaha FM, Casio CZ synthesizers generate sound by reading a cosine wave from memory and distorting the speed at which the phase angle is read.",
        "Produces crisp, punchy digital timbres ranging from warm analog-style brass and basses to glassy bells and metallic leads.",
        "A digital phase counter sweeps from 0 to 2*PI. When DCW (Distortion) = 0, reading is linear -> pure sine/cosine wave. As DCW increases, the phase slows down or speeds up mid-cycle, adding bright upper harmonics just like opening a low-pass filter!");

    add(Language::En, "dco",
        "DCO - Digitally Controlled Oscillator",
        "Controls Pitch & Pitch Envelope",
        "The DCO sets the fundamental frequency (pitch) of the note and lets you modulate pitch over time with an 8-stage Pitch Envelope.",
        "Creates pitch sweeps, laser zap effects, realistic brass lip-slurs, or natural acoustic attack transients.",
        "Controls the step increment of the phase accumulator. Modulating DCO changes the playback rate of the phase table (in semitones/cents) over the 8 envelope stages.");

    add(Language::En, "dcw",
        "DCW - Digitally Controlled Wave",
        "The 'Filter' of Phase Distortion",
        "DCW controls how much the waveform phase is distorted. It behaves musically like a resonant low-pass filter (VCF) in an analog synthesizer.",
        "Level 0 = dull/pure sine wave. High levels = bright, edgy, sawtooth or resonant-like harmonic content.",
        "Varies the non-linearity of the phase lookup angle. The 8-stage DCW envelope dynamically shapes brightness over time (e.g., bright attack decaying into a mellow sustain).");

    add(Language::En, "dca",
        "DCA - Digitally Controlled Amplifier",
        "Controls Volume & Loudness Contour",
        "The DCA shapes the overall amplitude (volume) of the sound from key press to release using an 8-stage envelope.",
        "Determines whether a patch is a percussive pluck, a slow-swelling pad, or a sustained organ.",
        "Multiplies the final waveform sample by the 8-stage amplitude envelope value before output mixing.");

    add(Language::En, "envelope",
        "8-Stage CZ Envelope (Rate & Level)",
        "Beyond standard ADSR",
        "Every DCO, DCW, and DCA has up to 8 programmable stages. Each stage defines a Rate (speed: 00-99) to reach a target Level (00-99).",
        "Allows complex multi-peak swells, bouncing echoes, stepped rhythmic shapes, or classic ADSR contours.",
        "Rate 99 is instantaneous; Rate 00 is slowest. Level 99 is maximum; Level 00 is silence/zero. You can place a Sustain point and an End step anywhere from step 1 to 8.");

    add(Language::En, "sustain",
        "Sustain Point (SUS)",
        "Where the note holds while key is down",
        "Marks the envelope step where progression pauses as long as you hold the key down. When you release the key, the envelope jumps to the next step toward End.",
        "Without a Sustain point, the sound plays straight through to the End step regardless of how long you hold the key (ideal for drums/percussion).",
        "Only one step (1..8) can be set as Sustain, or None. In the CZ-3000, Sustain must be <= End Step.");

    add(Language::En, "end_step",
        "End Step (END)",
        "Final step of the envelope",
        "Defines which stage (1..8) terminates the envelope. Steps after End are ignored.",
        "Shortens unused stages so you can build fast 3-stage or 4-stage ADSR-like envelopes without waiting for 8 steps.",
        "When DCA reaches the End step, the voice finishes and is freed for polyphony.");

    add(Language::En, "waves",
        "33 PD Waveforms (W1 & W2)",
        "8 Base Shapes + Dual Combinations",
        "Each oscillator can alternate between Waveform 1 (W1) and Waveform 2 (W2) on every cycle, yielding 33 distinct harmonic spectra.",
        "Sawtooth (1) = brassy; Square (2) = hollow/woodwind; Pulse (3) = nasal; Double Sine (4) = octave/organ; Saw-Pulse (5) = rich strings; Resonance I/II/III (6-8) = resonant filter sweeps without an actual VCF!",
        "The 3 Resonance waveforms multiply a higher-frequency cosine inside a triangular window synchronized to the fundamental, creating formant/resonant peaks via pure phase math.");

    add(Language::En, "line_select",
        "Line Select (1, 2, 1+2', 1+1')",
        "Oscillator routing & polyphony tradeoff",
        "Selects whether a patch uses 1 oscillator line (16-voice polyphony) or 2 stacked/detuned oscillator lines (8-voice polyphony).",
        "Line 1 or 2 = 16 voices, clean single-oscillator sound. Line 1+2' = 8 voices, thick dual-oscillator chorused/detuned or layered sound. Line 1+1' = doubles Line 1 with itself.",
        "In 2-line modes (1+2' or 1+1'), each note allocates two PD voice channels on the CZ-3000 hardware, halving max polyphony from 16 to 8.");

    add(Language::En, "detune",
        "Detune (+/- Oct, Note, Fine)",
        "Pitch offset between Line 1 and Line 2'",
        "Shifts the pitch of the second line (Line 2' or 1') relative to Line 1 in Octaves, Semitones, and Fine cents (0-60).",
        "Small Fine detune (02-08) creates lush chorus/analog fatness. +12 semitones or +7 (fifth) creates power-chord or harmonic organ intervals.",
        "Adds a fixed phase-increment offset to the secondary oscillator channel.");

    add(Language::En, "vibrato",
        "Vibrato (LFO)",
        "Cyclic pitch modulation",
        "Applies a low-frequency oscillation (Wave 1-4: Triangle, Saw Up, Saw Down, Square) to pitch after an optional Delay.",
        "Adds natural acoustic expression to strings/leads, or sci-fi trills and siren effects with Square wave and high Depth.",
        "Delay (00-99) waits before fading in modulation; Rate (00-99) sets LFO speed; Depth (00-99) sets pitch deviation.");

    add(Language::En, "key_follow",
        "Key Follow (DCW & DCA 0-9)",
        "Keyboard tracking for brightness & decay",
        "Adjusts how DCW brightness and DCA envelope speed respond across the keyboard from low bass notes to high treble notes.",
        "DCW Key Follow lowers distortion on high notes to prevent harsh aliasing or keep acoustic warmth. DCA Key Follow makes high notes decay faster like a real piano or harp.",
        "Scales DCW level and DCA rate proportionally to the MIDI note number.");

    add(Language::En, "ring_noise",
        "Ring & Noise Modulation",
        "Metallic bells, clang & breath/wind",
        "Cross-modulates Line 1 and Line 2' (Ring Mod) or mixes digital noise modulation (Noise Mod) into the phase.",
        "Ring Mod creates inharmonic bell, gong, and chime tones. Noise Mod adds snare snap, breathiness, or gritty percussion textures.",
        "Ring Modulation multiplies the two oscillator outputs; Noise Modulation injects a pseudo-random shift register into the phase/amplitude path.");

    add(Language::En, "sysex_io",
        "SysEx & CZ-3000 MIDI Bank",
        "Real-time Edit Buffer & Memory Transfers",
        "Communicates directly with the Casio CZ-3000 via MIDI System Exclusive (SysEx) 263/264-byte patch messages.",
        "Every slider move can update the CZ-3000 Edit Buffer live, or pull/push the 32 Internal Presets, 32 User Memories, and `.syx` bank files.",
        "Casio CZ SysEx encodes each parameter in 4-bit nibbles (low nibble first) with header `F0 44 00 00 7x ... F7`.");

    // =========================================================================
    // SPANISH (Es)
    // =========================================================================
    add(Language::Es, "intro",
        "¿Qué es la Distorsión de Fase (PD)?",
        "Síntesis digital clásica de Casio (1984-1985)",
        "A diferencia de los sintetizadores analógicos (VCO -> VCF -> VCA) o la síntesis FM de Yamaha, el Casio CZ genera sonido leyendo una onda coseno de memoria y distorsionando la velocidad con la que recorre su ángulo de fase.",
        "Produce timbres digitales nítidos y con pegada, desde metales y bajos estilo analógico hasta campanas cristalinas y leads metálicos.",
        "Un contador digital de fase avanza de 0 a 2*PI. Con DCW = 0 la lectura es lineal (onda pura). Al subir DCW, la fase se frena o acelera a mitad de ciclo, generando armónicos brillantes igual que al abrir un filtro paso bajo.");

    add(Language::Es, "dco",
        "DCO - Oscilador Controlado Digitalmente",
        "Controla la Afinación y su Envolvente",
        "El DCO define la frecuencia fundamental (tono) de la nota y permite modular la afinación en el tiempo mediante una Envolvente de Tono de 8 etapas.",
        "Crea barridos de tono, efectos láser, ligados expresivos de metal o transitorios acústicos naturales en el ataque.",
        "Controla el incremento de paso del acumulador de fase. Modular el DCO cambia la velocidad de lectura de la tabla de fase (en semitonos/cents) en 8 etapas.");

    add(Language::Es, "dcw",
        "DCW - Onda Controlada Digitalmente",
        "El 'Filtro' de la Distorsión de Fase",
        "DCW controla cuánta distorsión se aplica a la fase de la onda. Musicalmente se comporta como un filtro paso bajo resonante (VCF) analógico.",
        "Nivel 0 = onda senoidal pura y opaca. Niveles altos = sonido brillante, agresivo, tipo diente de sierra o resonante.",
        "Varía la no-linealidad del ángulo de lectura de fase. La envolvente DCW de 8 pasos moldea el brillo en el tiempo (ej. ataque brillante que decae a un sostenido suave).");

    add(Language::Es, "dca",
        "DCA - Amplificador Controlado Digitalmente",
        "Controla el Volumen y la Dinámica",
        "El DCA moldea la amplitud (volumen) total del sonido desde que pulsas la tecla hasta que la sueltas mediante una envolvente de 8 pasos.",
        "Define si el timbre es un punteo percusivo, un colchón de ataque lento o un órgano sostenido.",
        "Multiplica la muestra final de la onda por el valor de la envolvente de amplitud de 8 etapas antes de la mezcla de salida.");

    add(Language::Es, "envelope",
        "Envolvente CZ de 8 Etapas (Rate y Level)",
        "Más allá del ADSR tradicional",
        "Cada sección DCO, DCW y DCA tiene hasta 8 etapas programables. Cada etapa define una Velocidad (Rate: 00-99) para alcanzar un Nivel destino (Level: 00-99).",
        "Permite crecendos complejos con varios picos, ecos rebotados, formas rítmicas escalonadas o contornos ADSR clásicos.",
        "Rate 99 es instantáneo; Rate 00 es el más lento. Level 99 es el máximo; Level 00 es silencio/cero. Puedes fijar un punto Sustain y un End Step entre el paso 1 y el 8.");

    add(Language::Es, "sustain",
        "Punto de Sustain (SUS)",
        "Dónde se mantiene la nota al pulsar la tecla",
        "Marca el paso donde la envolvente se detiene mientras mantienes la tecla pulsada. Al soltarla, salta hacia el siguiente paso rumbo al End Step.",
        "Sin punto de Sustain, el sonido recorre toda la envolvente hasta End sin importar cuánto mantengas la tecla (ideal para percusión).",
        "Solo un paso (1..8) puede marcarse como Sustain, o ninguno (None). En el CZ-3000, Sustain debe ser <= End Step.");

    add(Language::Es, "end_step",
        "Paso Final (END)",
        "Última etapa activa de la envolvente",
        "Define qué etapa (1..8) termina la envolvente. Los pasos posteriores a End se ignoran.",
        "Acorta etapas sin usar para construir envolventes rápidas de 3 o 4 pasos tipo ADSR sin esperar los 8 pasos.",
        "Cuando el DCA alcanza el paso End, la voz finaliza y queda libre para la polifonía.");

    add(Language::Es, "waves",
        "33 Formas de Onda PD (W1 y W2)",
        "8 Formas Base + Combinaciones Duales",
        "Cada oscilador puede alternar entre Onda 1 (W1) y Onda 2 (W2) en cada ciclo, logrando 33 espectros armónicos distintos.",
        "Saw (1) = metales; Square (2) = hueco/viento madera; Pulse (3) = nasal; Double Sine (4) = octava/órgano; Saw-Pulse (5) = cuerdas ricas; Resonance I/II/III (6-8) = ¡barridos resonantes sin VCF!",
        "Las 3 ondas Resonance multiplican un coseno de alta frecuencia dentro de una ventana triangular sincronizada a la fundamental, creando formantes mediante matemática de fase.");

    add(Language::Es, "line_select",
        "Selección de Línea (1, 2, 1+2', 1+1')",
        "Enrutamiento de osciladores y polifonía",
        "Elige si el patch usa 1 línea de oscilador (16 voces de polifonía) o 2 líneas apiladas/desafinadas (8 voces de polifonía).",
        "Line 1 o 2 = 16 voces, sonido limpio de un oscilador. Line 1+2' = 8 voces, sonido grueso con chorus/detune o capas. Line 1+1' = duplica la Línea 1 consigo misma.",
        "En modos de 2 líneas (1+2' o 1+1'), cada nota asigna dos canales de voz PD en el hardware del CZ-3000, reduciendo la polifonía máxima de 16 a 8.");

    add(Language::Es, "detune",
        "Detune (+/- Oct, Note, Fine)",
        "Desplazamiento tonal entre Línea 1 y Línea 2'",
        "Desplaza la afinación de la segunda línea (Línea 2' o 1') respecto a la Línea 1 en Octavas, Semitonos y ajuste Fino en cents (0-60).",
        "Un Detune fino pequeño (02-08) da grosor analógico/chorus. +12 semitonos o +7 (quinta) crean intervalos de órgano o quintas potentes.",
        "Suma un desplazamiento fijo de incremento de fase al canal del oscilador secundario.");

    add(Language::Es, "vibrato",
        "Vibrato (LFO)",
        "Modulación cíclica de afinación",
        "Aplica una oscilación de baja frecuencia (Onda 1-4: Triangular, Sierra Arriba, Sierra Abajo, Cuadrada) al tono tras un Retardo (Delay) opcional.",
        "Aporta expresión acústica natural a cuerdas/leads, o trinos sci-fi y sirenas con onda Cuadrada y Depth alto.",
        "Delay (00-99) espera antes de introducir la modulación; Rate (00-99) fija la velocidad del LFO; Depth (00-99) fija la profundidad.");

    add(Language::Es, "key_follow",
        "Key Follow (DCW y DCA 0-9)",
        "Seguimiento de teclado para brillo y decaimiento",
        "Ajusta cómo responden el brillo del DCW y la velocidad de la envolvente DCA a lo largo del teclado, desde graves hasta agudos.",
        "DCW Key Follow suaviza los agudos para evitar estridencia y mantener calidez. DCA Key Follow hace que las notas altas decaigan más rápido, como un piano o arpa real.",
        "Escala el nivel DCW y la velocidad DCA proporcionalmente al número de nota MIDI.");

    add(Language::Es, "ring_noise",
        "Modulación Ring y Noise",
        "Campanas metálicas, percusión y aire",
        "Modula en anillo la Línea 1 con la Línea 2' (Ring Mod) o mezcla modulación de ruido digital (Noise Mod) en la fase.",
        "Ring Mod crea campanas inarmónicas, gongs y timbres metálicos. Noise Mod añade chasquido de caja, soplo o texturas percusivas.",
        "Ring Modulation multiplica las salidas de ambos osciladores; Noise Modulation inyecta un registro pseudoaleatorio en la ruta de fase/amplitud.");

    add(Language::Es, "sysex_io",
        "SysEx y Banco MIDI CZ-3000",
        "Buffer de Edición en vivo y Transferencia de Memorias",
        "Comunica directamente con el Casio CZ-3000 mediante mensajes MIDI System Exclusive (SysEx) de 263/264 bytes por patch.",
        "Cada ajuste puede actualizar el Edit Buffer del CZ-3000 en vivo, o leer/enviar los 32 Presets internos, 32 Memorias de usuario y archivos `.syx`.",
        "El SysEx de Casio CZ codifica cada parámetro en nibbles de 4 bits (nibble bajo primero) con cabecera `F0 44 00 00 7x ... F7`.");

    // =========================================================================
    // ITALIAN (It)
    // =========================================================================
    add(Language::It, "intro",
        "Cos'è la Distorsione di Fase (PD)?",
        "Sintesi digitale classica Casio (1984-1985)",
        "A differenza dei synth analogici (VCO -> VCF -> VCA) o della sintesi FM Yamaha, il Casio CZ genera il suono leggendo un'onda coseno dalla memoria e distorcendo la velocità di lettura dell'angolo di fase.",
        "Produce timbri digitali nitidi e incisivi: dai bassi e ottoni in stile analogico fino a campane cristalline e lead metallici.",
        "Un contatore digitale di fase avanza da 0 a 2*PI. Con DCW = 0 la lettura è lineare (sinusoide pura). Aumentando DCW, la fase rallenta o accelera a metà ciclo, creando armoniche brillanti come aprendo un filtro passa-basso.");

    add(Language::It, "dco",
        "DCO - Oscillatore Controllato Digitalmente",
        "Controlla Intonazione e Inviluppo Pitch",
        "Il DCO imposta la frequenza fondamentale (pitch) della nota e consente di modularla nel tempo tramite un inviluppo Pitch a 8 stadi.",
        "Crea sweep di intonazione, effetti laser, legature realistiche di ottoni o transienti acustici naturali.",
        "Controlla l'incremento di passo dell'accumulatore di fase. Modulare il DCO varia la velocità di lettura della tabella di fase negli 8 stadi.");

    add(Language::It, "dcw",
        "DCW - Onda Controllata Digitalmente",
        "Il 'Filtro' della Distorsione di Fase",
        "Il DCW regola la quantità di distorsione applicata alla fase dell'onda. Si comporta musicalmente come un filtro passa-basso risonante (VCF).",
        "Livello 0 = onda sinusoidale pura e scura. Livelli alti = suono brillante, tagliente, tipo dente di sega o risonante.",
        "Varia la non-linearità dell'angolo di lettura di fase. L'inviluppo DCW a 8 stadi modella la brillantezza nel tempo.");

    add(Language::It, "dca",
        "DCA - Amplificatore Controllato Digitalmente",
        "Controlla Volume e Dinamica",
        "Il DCA modella l'ampiezza (volume) complessiva del suono dalla pressione del tasto al rilascio tramite un inviluppo a 8 stadi.",
        "Determina se il suono è un pizzicato percussivo, un pad ad attacco lento o un organo sostenuto.",
        "Moltiplica il campione finale dell'onda per il valore dell'inviluppo di ampiezza a 8 stadi prima del mix finale.");

    add(Language::It, "envelope",
        "Inviluppo CZ a 8 Stadi (Rate & Level)",
        "Oltre il classico ADSR",
        "Ogni sezione DCO, DCW e DCA dispone di 8 stadi programmabili. Ogni stadio definisce una Velocità (Rate: 00-99) per raggiungere un Livello (Level: 00-99).",
        "Consente inviluppi complessi multi-picco, echi ribattuti, forme ritmiche a gradini o classici profili ADSR.",
        "Rate 99 è istantaneo; Rate 00 è il più lento. Level 99 è massimo; Level 00 è silenzio. Puoi impostare Sustain ed End Step dal passo 1 all'8.");

    add(Language::It, "sustain",
        "Punto di Sustain (SUS)",
        "Dove la nota rimane tenuta",
        "Indica lo stadio in cui l'inviluppo si ferma finché tieni premuto il tasto. Al rilascio, prosegue verso lo stadio successivo fino a End.",
        "Senza Sustain, il suono attraversa tutto l'inviluppo fino a End indipendentemente dalla durata della pressione (ideale per percussioni).",
        "Solo uno stadio (1..8) può essere impostato come Sustain, oppure Nessuno. Nel CZ-3000, Sustain deve essere <= End Step.");

    add(Language::It, "end_step",
        "Passo Finale (END)",
        "Ultimo stadio attivo dell'inviluppo",
        "Definisce quale stadio (1..8) conclude l'inviluppo. Gli stadi successivi vengono ignorati.",
        "Accorcia gli stadi inutilizzati per creare inviluppi veloci a 3 o 4 passi tipo ADSR senza attendere 8 stadi.",
        "Quando il DCA raggiunge il passo End, la voce termina e torna disponibile per la polifonia.");

    add(Language::It, "waves",
        "33 Forme d'Onda PD (W1 & W2)",
        "8 Forme Base + Combinazioni Duali",
        "Ogni oscillatore può alternare Onda 1 (W1) e Onda 2 (W2) ad ogni ciclo, offrendo 33 spettri armonici distinti.",
        "Saw (1) = ottoni; Square (2) = legni/suoni cavi; Pulse (3) = nasale; Double Sine (4) = ottava/organo; Saw-Pulse (5) = archi ricchi; Resonance I/II/III (6-8) = sweep risonanti senza VCF!",
        "Le 3 forme Resonance moltiplicano un coseno ad alta frequenza dentro una finestra triangolare sincronizzata alla fondamentale.");

    add(Language::It, "line_select",
        "Selezione Linea (1, 2, 1+2', 1+1')",
        "Routing oscillatori e polifonia",
        "Seleziona se il timbro usa 1 linea oscillatore (16 voci di polifonia) o 2 linee sovrapposte/scordate (8 voci di polifonia).",
        "Line 1 o 2 = 16 voci, suono pulito a singolo oscillatore. Line 1+2' = 8 voci, suono spesso con chorus/detune o layer. Line 1+1' = raddoppia la Linea 1.",
        "Nelle modalità a 2 linee (1+2' o 1+1'), ogni nota impegna due canali PD sul CZ-3000, dimezzando la polifonia da 16 a 8.");

    add(Language::It, "detune",
        "Detune (+/- Oct, Note, Fine)",
        "Scostamento di intonazione tra Linea 1 e 2'",
        "Sposta l'intonazione della seconda linea (2' o 1') rispetto alla Linea 1 in Ottave, Semitoni e Centesimi Fine (0-60).",
        "Un Detune fine leggero (02-08) crea spessore analogico/chorus. +12 semitoni o +7 (quinta) creano intervalli armonici.",
        "Aggiunge un offset fisso di incremento fase al canale dell'oscillatore secondario.");

    add(Language::It, "vibrato",
        "Vibrato (LFO)",
        "Modulazione ciclica dell'intonazione",
        "Applica un'oscillazione a bassa frequenza (Onda 1-4: Triangolo, Dente su, Dente giù, Quadra) al pitch dopo un Delay opzionale.",
        "Aggiunge espressività naturale ad archi e lead, oppure trilli sci-fi e sirene con onda Quadra e Depth elevato.",
        "Delay (00-99) attende prima di inserire la modulazione; Rate (00-99) regola la velocità; Depth (00-99) l'intensità.");

    add(Language::It, "key_follow",
        "Key Follow (DCW & DCA 0-9)",
        "Tracking da tastiera per brillantezza e decadimento",
        "Regola come la brillantezza DCW e la velocità DCA rispondono lungo la tastiera dai bassi agli acuti.",
        "DCW Key Follow attenua gli acuti per evitare asprezza digitale. DCA Key Follow accelera il decadimento sulle note alte come in un pianoforte reale.",
        "Scala il livello DCW e la velocità DCA in proporzione al numero di nota MIDI.");

    add(Language::It, "ring_noise",
        "Modulazione Ring & Noise",
        "Campane metalliche, clangore e soffio",
        "Modula ad anello Linea 1 e Linea 2' (Ring Mod) oppure miscela modulazione di rumore digitale (Noise Mod) nella fase.",
        "Ring Mod produce campane inarmoniche e gong. Noise Mod aggiunge attacco percussivo, soffio o ruvidità.",
        "Ring Modulation moltiplica le uscite dei due oscillatori; Noise Modulation inietta un registro pseudo-casuale nel percorso di fase/ampiezza.");

    add(Language::It, "sysex_io",
        "SysEx & Banco MIDI CZ-3000",
        "Buffer di modifica live e trasferimento memorie",
        "Comunica direttamente con il Casio CZ-3000 tramite messaggi MIDI System Exclusive (SysEx) da 263/264 byte per patch.",
        "Ogni modifica aggiorna l'Edit Buffer del CZ-3000 in tempo reale, oppure legge/invia i 32 Preset, le 32 Memorie Utente e i file `.syx`.",
        "Il SysEx Casio CZ codifica ogni parametro in nibble a 4 bit (nibble basso prima) con header `F0 44 00 00 7x ... F7`.");

    // =========================================================================
    // GERMAN (De)
    // =========================================================================
    add(Language::De, "intro",
        "Was ist Phase Distortion (PD)?",
        "Casios digitale Synthese (1984-1985)",
        "Anders als analoge Synthesizer (VCO -> VCF -> VCA) oder Yamahas FM erzeugt der Casio CZ Klänge, indem er eine Kosinuswelle ausliest und die Geschwindigkeit des Phasenwinkels verzerrt.",
        "Liefert präzise, druckvolle Digitalklänge von warmen Analog-Bässen und Brass bis zu glasigen Glocken und metallischen Leads.",
        "Ein digitaler Phasenzähler läuft von 0 bis 2*PI. Bei DCW = 0 ist die Auslesung linear (reiner Sinus). Steigt DCW, verlangsamt/beschleunigt sich die Phase pro Zyklus und erzeugt Obertöne wie ein geöffnetes Tiefpassfilter.");

    add(Language::De, "dco",
        "DCO - Digital gesteuerter Oszillator",
        "Steuert Tonhöhe & Pitch-Hüllkurve",
        "Der DCO bestimmt die Grundfrequenz (Tonhöhe) und ermöglicht zeitliche Tonhöhenmodulation über eine 8-stufige Pitch-Hüllkurve.",
        "Erzeugt Pitch-Sweeps, Laser-Effekte, realistische Bläser-Ansätze oder natürliche Einschwingvorgänge.",
        "Steuert die Schrittweite des Phasenakkumulators. Die 8-stufige DCO-Hüllkurve verschiebt die Auslesegeschwindigkeit in Halbtönen/Cents.");

    add(Language::De, "dcw",
        "DCW - Digital gesteuerte Welle",
        "Das 'Filter' der Phase Distortion",
        "DCW steuert die Stärke der Phasenverzerrung. Musikalisch verhält es sich wie ein resonantes Tiefpassfilter (VCF) im Analog-Synth.",
        "Wert 0 = dumpfer, reiner Sinus. Hohe Werte = brillant, obertonreich, sägezahn- oder resonanzartig.",
        "Verändert die Nichtlinearität des Phasenwinkels. Die 8-stufige DCW-Hüllkurve formt die Klanghelligkeit über die Zeit.");

    add(Language::De, "dca",
        "DCA - Digital gesteuerter Verstärker",
        "Steuert Lautstärke & Dynamikverlauf",
        "Der DCA formt die Gesamtlautstärke vom Tastenanschlag bis zum Ausklingen über eine 8-stufige Hüllkurve.",
        "Bestimmt, ob ein Sound ein perkussives Zupfen, ein langsam anschwellendes Pad oder eine gehaltene Orgel ist.",
        "Multipliziert das fertige Wellensignal mit dem aktuellen Wert der 8-stufigen Amplitudenhüllkurve.");

    add(Language::De, "envelope",
        "8-Stufen CZ-Hüllkurve (Rate & Level)",
        "Mehr als klassisches ADSR",
        "Jeder DCO, DCW und DCA besitzt bis zu 8 programmierbare Stufen mit Geschwindigkeit (Rate: 00-99) und Zielwert (Level: 00-99).",
        "Ermöglicht komplexe Mehrspitzen-Verläufe, Echo-Effekte, rhythmische Stufen oder klassische ADSR-Konturen.",
        "Rate 99 ist sofortig; Rate 00 am langsamsten. Level 99 ist Maximum; Level 00 ist Stille. Sustain und End Step sind frei zwischen 1 und 8 wählbar.");

    add(Language::De, "sustain",
        "Sustain-Punkt (SUS)",
        "Haltepunkt bei gedrückter Taste",
        "Markiert die Stufe, an der die Hüllkurve anhält, solange die Taste gedrückt bleibt. Beim Loslassen springt sie zum nächsten Schritt Richtung End.",
        "Ohne Sustain läuft die Hüllkurve unabhängig von der Tasten-Haltedauer komplett bis End durch (ideal für Drums/Percussion).",
        "Nur eine Stufe (1..8) kann als Sustain gesetzt werden, oder Keine. Beim CZ-3000 gilt Sustain <= End Step.");

    add(Language::De, "end_step",
        "End-Schritt (END)",
        "Letzte aktive Stufe der Hüllkurve",
        "Legt fest, bei welcher Stufe (1..8) die Hüllkurve endet. Nachfolgende Stufen werden ignoriert.",
        "Verkürzt ungenutzte Stufen, um knackige 3- oder 4-Stufen-ADSR-Verläufe ohne Wartezeit zu bauen.",
        "Erreicht der DCA den End-Schritt, wird die Stimme beendet und für die Polyphonie freigegeben.");

    add(Language::De, "waves",
        "33 PD-Wellenformen (W1 & W2)",
        "8 Grundformen + Dual-Kombinationen",
        "Jeder Oszillator kann zyklusweise zwischen Welle 1 (W1) und Welle 2 (W2) wechseln – das ergibt 33 Obertonspektren.",
        "Saw (1) = Brass; Square (2) = hohl/Holzbläser; Pulse (3) = nasal; Double Sine (4) = Oktave/Orgel; Saw-Pulse (5) = dichte Strings; Resonance I/II/III (6-8) = Resonanz-Sweeps ohne VCF!",
        "Die 3 Resonance-Wellen multiplizieren einen hochfrequenten Kosinus in einem Dreiecksfenster synchron zur Grundfrequenz.");

    add(Language::De, "line_select",
        "Line Select (1, 2, 1+2', 1+1')",
        "Oszillator-Routing & Polyphonie",
        "Wählt zwischen 1 Oszillator-Linie (16 Stimmen Polyphonie) oder 2 gestapelten/verstimmten Linien (8 Stimmen Polyphonie).",
        "Line 1 oder 2 = 16 Stimmen, klarer Ein-Oszillator-Klang. Line 1+2' = 8 Stimmen, breiter Chorus/Detune- oder Layer-Klang. Line 1+1' = doppelt Line 1.",
        "Im 2-Linien-Modus (1+2' oder 1+1') belegt jede Note zwei PD-Stimmenkanäle im CZ-3000, wodurch sich die Polyphonie von 16 auf 8 halbiert.");

    add(Language::De, "detune",
        "Detune (+/- Oct, Note, Fine)",
        "Tonhöhenversatz zwischen Line 1 und 2'",
        "Verschiebt die Tonhöhe der zweiten Linie (2' oder 1') relativ zu Line 1 in Oktaven, Halbtönen und Fine-Cents (0-60).",
        "Leichtes Fine-Detune (02-08) erzeugt analogen Chorus-Druck. +12 oder +7 Halbtöne (Quinte) liefern Orgel- oder Powerchord-Intervalle.",
        "Addiert einen festen Phaseninkrement-Offset zum zweiten Oszillatorkanal.");

    add(Language::De, "vibrato",
        "Vibrato (LFO)",
        "Zyklische Tonhöhenmodulation",
        "Moduliert die Tonhöhe nach optionalem Delay mit einer LFO-Welle (1-4: Dreieck, Säge auf, Säge ab, Rechteck).",
        "Verleiht Streichern/Leads natürlichen Ausdruck oder erzeugt mit Rechteck und hohem Depth Sci-Fi-Triller.",
        "Delay (00-99) verzögert das Einsetzen; Rate (00-99) bestimmt die LFO-Frequenz; Depth (00-99) die Stärke.");

    add(Language::De, "key_follow",
        "Key Follow (DCW & DCA 0-9)",
        "Tastatur-Tracking für Helligkeit & Ausklingen",
        "Steuert, wie stark DCW-Helligkeit und DCA-Geschwindigkeit über den Tastaturbereich von Bass bis Diskant skalieren.",
        "DCW Key Follow zähmt schrille Höhen. DCA Key Follow lässt hohe Töne schneller ausklingen – wie bei einem echten Klavier.",
        "Skaliert DCW-Level und DCA-Rate proportional zur MIDI-Notennummer.");

    add(Language::De, "ring_noise",
        "Ring- & Noise-Modulation",
        "Metallische Glocken, Gong & Rauschen",
        "Ringmoduliert Line 1 und Line 2' (Ring Mod) oder mischt digitales Rauschen (Noise Mod) in die Phase.",
        "Ring Mod erzeugt unharmonische Glocken und Gongs. Noise Mod liefert Snare-Attack, Atemgeräusche oder perkussive Texturen.",
        "Ring Modulation multipliziert beide Oszillator-Ausgänge; Noise Modulation speist ein Schieberegister in den Phasen-/Amplitudenpfad ein.");

    add(Language::De, "sysex_io",
        "SysEx & CZ-3000 MIDI-Bank",
        "Live Edit Buffer & Speicherübertragung",
        "Kommuniziert direkt mit dem Casio CZ-3000 per MIDI System Exclusive (SysEx) in 263/264-Byte-Patchpaketen.",
        "Reglerbewegungen aktualisieren den CZ-3000 Edit Buffer live oder laden/senden die 32 Presets, 32 User-Memories und `.syx`-Dateien.",
        "Casio CZ SysEx kodiert jeden Parameter in 4-Bit-Nibbles (Low-Nibble zuerst) mit Header `F0 44 00 00 7x ... F7`.");

    // =========================================================================
    // JAPANESE (Ja)
    // =========================================================================
    add(Language::Ja, "intro",
        "フェイズ・ディストーション (PD) 音源とは？",
        "カシオ独自のデジタルシンセシス (1984-1985)",
        "従来のアナログシンセ (VCO -> VCF -> VCA) やヤマハのFM音源とは異なり、CZシリーズはメモリ内の余弦波(コサイン波)を読み出す際の位相角の進み方を歪ませることで音色を作ります。",
        "アナログ風の太いブラスやベースから、透明感のあるベルや金属的なリードまで、輪郭のはっきりしたサウンドを生み出します。",
        "位相カウンタは0から2*PIへ進みます。DCW=0では直線的に読み出され純粋なサイン波になります。DCWを上げると周期途中で位相の進みが遅速し、ローパスフィルタを開いたような倍音が生まれます。");

    add(Language::Ja, "dco",
        "DCO - デジタル制御オシレーター",
        "音程とピッチエンベロープを制御",
        "DCOは音の基本周波数(ピッチ)を決め、8段階のピッチエンベロープによって時間的な音程変化を作り出します。",
        "ピッチベンド風のスウィープ、レーザー効果音、ブラスの自然な立ち上がりなどを表現できます。",
        "位相アキュムレータの増分を制御し、8ステップのエンベロープで半音/セント単位の音程変化を与えます。");

    add(Language::Ja, "dcw",
        "DCW - デジタル制御ウェーブ",
        "PD音源における「フィルター」",
        "DCWは波形の位相をどれだけ歪ませるかを制御します。アナログシンセのレゾナンス付きローパスフィルター(VCF)のように機能します。",
        "レベル0＝こもった純粋なサイン波。高レベル＝明るく鋭いノコギリ波やレゾナンス感のある倍音構成。",
        "位相ルックアップ角の非線形性を変化させ、8段階のDCWエンベロープで時間とともに音色の明るさを変化させます。");

    add(Language::Ja, "dca",
        "DCA - デジタル制御アンプリファイア",
        "音量とダイナミクスを制御",
        "DCAは鍵盤を押してから離すまでの音量変化を8段階のエンベロープで形作ります。",
        "打楽器的なプラック音、ゆっくり立ち上がるパッド、持続するオルガンなどの音の輪郭を決めます。",
        "最終的な波形サンプルに8段階の振幅エンベロープ値を乗算して出力します。");

    add(Language::Ja, "envelope",
        "8ステップ CZエンベロープ (Rate & Level)",
        "従来のADSRを超える自由な設計",
        "DCO・DCW・DCAのすべてに最大8ステップのエンベロープがあり、各ステップで目標値(Level: 00-99)への到達速度(Rate: 00-99)を設定します。",
        "複数の山を持つ複雑な変化、エコー風の反復、ステップ状の音色変化、定番のADSRカーブまで自在に作れます。",
        "Rate 99が最速(瞬間)、00が最遅。Level 99が最大、00が無音/ゼロです。ステップ1〜8の任意の位置にサスティンとエンドを設定できます。");

    add(Language::Ja, "sustain",
        "サスティンポイント (SUS)",
        "鍵盤を押している間の保持位置",
        "鍵盤を押し続けている間、エンベロープの進行が一時停止するステップを指定します。鍵盤を離すと次のステップからEndへ向かいます。",
        "サスティンを設定しない(None)場合、鍵盤を押す長さに関係なくEndまで一気に再生されます(ドラムやパーカッション向け)。",
        "1〜8のいずれか1つ(またはNone)をサスティンに設定できます。CZ-3000では Sustain <= End Step である必要があります。");

    add(Language::Ja, "end_step",
        "エンドステップ (END)",
        "エンベロープの終了位置",
        "エンベロープを終了させるステップ(1〜8)を指定します。Endより後のステップは無視されます。",
        "不要なステップを省き、3〜4ステップの素早いADSR型エンベロープを簡潔に作成できます。",
        "DCAがEndステップに到達すると発音が終了し、ポリフォニーのボイスが解放されます。");

    add(Language::Ja, "waves",
        "33種類のPD波形 (W1 & W2)",
        "8種の基本波形 + デュアル組み合わせ",
        "各オシレーターは周期ごとに波形1(W1)と波形2(W2)を交互に切り替え、合計33種類の倍音スペクトルを生成します。",
        "Saw(1)=ブラス、Square(2)=木管、Pulse(3)=鼻にかかった音、Double Sine(4)=オクターブ/オルガン、Saw-Pulse(5)=厚いストリングス、Resonance I〜III(6-8)=VCF不要のレゾナンス波形！",
        "3種のResonance波形は、基音に同期した三角窓の中で高周波コサイン波を乗算し、位相演算だけでフォルマント/レゾナンスのピークを作ります。");

    add(Language::Ja, "line_select",
        "ラインセレクト (1, 2, 1+2', 1+1')",
        "オシレーター構成と同時発音数の関係",
        "1系統のオシレーター(最大16音ポリ)を使うか、2系統を重ねる/デチューンする(最大8音ポリ)かを選択します。",
        "Line 1または2＝16音ポリ・明瞭な単一音。Line 1+2'＝8音ポリ・厚いコーラス/デチューンやレイヤー音。Line 1+1'＝Line 1の二重化。",
        "2ラインモード(1+2' または 1+1')では1音あたりCZ-3000内部のPDボイスを2チャンネル消費するため、最大同時発音数が16から8に半減します。");

    add(Language::Ja, "detune",
        "デチューン (+/- Oct, Note, Fine)",
        "Line 1とLine 2'の音程差",
        "第2ライン(2'または1')の音程をLine 1に対してオクターブ、半音、ファイン(0-60セント)でずらします。",
        "わずかなFineデチューン(02-08)で豊かなコーラス感やアナログ的な太さが出ます。+12や+7(5度)でオルガンやパワーコード的な響きになります。",
        "第2オシレーターチャンネルの位相増分に一定のオフセットを加算します。");

    add(Language::Ja, "vibrato",
        "ビブラート (LFO)",
        "周期的なピッチ変調",
        "任意のディレイ時間の後、低周波オシレーター(波形1-4: 三角波、上昇ノコギリ、下降ノコギリ、矩形波)で音程を揺らします。",
        "ストリングスやリードに自然な表現力を与えたり、矩形波と深いDepthでSF風のトリルやサイレン音を作れます。",
        "Delay(00-99)で掛かり始めを遅らせ、Rate(00-99)で速さ、Depth(00-99)で揺れの深さを設定します。");

    add(Language::Ja, "key_follow",
        "キーフォロー (DCW & DCA 0-9)",
        "音域に応じた明るさと減衰の追従",
        "低音域から高音域にかけて、DCWの明るさとDCAエンベロープの速度がどう変化するかを調整します。",
        "DCWキーフォローは高音の耳障りさを抑えて温かみを保ちます。DCAキーフォローはピアノやハープのように高音ほど速く減衰させます。",
        "MIDIノート番号に比例してDCWレベルとDCAレートをスケーリングします。");

    add(Language::Ja, "ring_noise",
        "リング変調 & ノイズ変調",
        "金属的なベル、ゴング、息づかい",
        "Line 1とLine 2'を掛け合わせるリング変調(Ring Mod)、または位相にデジタルノイズを混ぜるノイズ変調(Noise Mod)を行います。",
        "Ring Modは非整数次倍音によるベルやゴングの音色を作り、Noise Modはスネアのアタックや息の成分、ざらついた打楽器音を加えます。",
        "リング変調は2つのオシレーター出力を乗算し、ノイズ変調は疑似乱数シフトレジスタを位相/振幅経路に注入します。");

    add(Language::Ja, "sysex_io",
        "SysEx & CZ-3000 MIDIバンク",
        "リアルタイム編集バッファ & メモリー転送",
        "MIDIシステム・エクスクルーシブ(SysEx)の263/264バイトのパッチデータでCZ-3000実機と直接通信します。",
        "スライダー操作でCZ-3000のEdit Bufferを即時更新したり、32プリセット・32ユーザーメモリー・`.syx`バンクファイルを送受信できます。",
        "Casio CZのSysExは各パラメーターを4ビットニブル(下位ニブル先)で符号化し、ヘッダ `F0 44 00 00 7x ... F7` を使用します。");

    // =========================================================================
    // SIMPLIFIED CHINESE (Zh)
    // =========================================================================
    add(Language::Zh, "intro",
        "什么是相位失真 (PD) 合成？",
        "卡西欧经典数字合成技术 (1984-1985)",
        "与传统模拟合成器 (VCO -> VCF -> VCA) 或雅马哈 FM 合成不同，Casio CZ 通过读取内存中的余弦波，并改变读取相位角的速度（扭曲相位）来生成声音。",
        "能产生清晰有力的数字音色，从温暖的模拟风格铜管、贝斯到晶莹的钟琴和金属感主音应有尽有。",
        "数字相位计数器从 0 扫描到 2*PI。当 DCW = 0 时线性读取（纯净正弦波）。提高 DCW 会使周期中段的相位变慢或加快，从而产生类似打开低通滤波器的明亮谐波。");

    add(Language::Zh, "dco",
        "DCO - 数字控制振荡器",
        "控制音高与音高包络",
        "DCO 决定音符的基频（音高），并允许通过 8 段音高包络随时间调制音高。",
        "可制作音高滑音、激光音效、逼真的铜管起音滑音或自然的原声乐器起音瞬态。",
        "控制相位累加器的步进增量。调制 DCO 会在 8 个包络阶段中以半音/音分为单位改变相位表的读取速率。");

    add(Language::Zh, "dcw",
        "DCW - 数字控制波形",
        "相位失真合成的“滤波器”",
        "DCW 控制波形相位的失真程度。在音乐表现上，它类似于模拟合成器中的谐振低通滤波器 (VCF)。",
        "等级 0 = 暗淡纯净的正弦波；高等级 = 明亮、锐利、锯齿波或带有谐振感的丰富泛音。",
        "改变相位查找角度的非线性程度。8 段 DCW 包络可随时间动态塑造音色明亮度（如明亮起音后衰减为柔和延音）。");

    add(Language::Zh, "dca",
        "DCA - 数字控制放大器",
        "控制音量与动态轮廓",
        "DCA 使用 8 段包络来塑造从按下琴键到松开琴键的整体振幅（音量）变化。",
        "决定音色是颗粒感拨弦、缓慢渐强的铺底音色，还是持续发声的风琴。",
        "在输出混音前，将最终波形采样值乘以 8 段振幅包络的当前值。");

    add(Language::Zh, "envelope",
        "8 段 CZ 包络 (Rate 与 Level)",
        "超越传统的 ADSR 包络",
        "DCO、DCW 和 DCA 各有最多 8 个可编程阶段。每个阶段定义一个速率 (Rate: 00-99) 以到达目标电平 (Level: 00-99)。",
        "可实现复杂的多峰值渐强、回声反弹、阶梯节奏型包络或经典 ADSR 轮廓。",
        "Rate 99 为瞬间到达，00 为最慢；Level 99 为最大，00 为静音/零。可在第 1 至 8 步任意设置延音点 (Sustain) 和结束步 (End)。");

    add(Language::Zh, "sustain",
        "延音点 (SUS)",
        "按住琴键时的保持位置",
        "标记在按住琴键期间包络暂停推进的步骤。松开琴键后，包络会从下一步继续走向 End。",
        "如果不设延音点，无论按住琴键多久，声音都会直接走完至 End 步（非常适合鼓和打击乐）。",
        "仅可将 1..8 中的某一步设为 Sustain（或设为无）。在 CZ-3000 中，Sustain 必须 <= End Step。");

    add(Language::Zh, "end_step",
        "结束步 (END)",
        "包络的最后有效阶段",
        "定义包络在第几步 (1..8) 结束。End 之后的步骤将被忽略。",
        "可缩短未使用的阶段，轻松构建 3 步或 4 步的快速 ADSR 包络而无需等待全部 8 步。",
        "当 DCA 到达 End 步时，该复音通道结束发声并释放资源。");

    add(Language::Zh, "waves",
        "33 种 PD 波形 (W1 与 W2)",
        "8 种基础波形 + 双波形组合",
        "每个振荡器可在每个周期交替使用波形 1 (W1) 和波形 2 (W2)，组合出 33 种独特的谐波频谱。",
        "Saw(1)=铜管；Square(2)=空灵/木管；Pulse(3)=鼻音感；Double Sine(4)=八度/风琴；Saw-Pulse(5)=丰满弦乐；Resonance I/II/III(6-8)=无需 VCF 即可实现谐振扫频！",
        "3 种 Resonance 波形在同步于基频的三角窗内乘以高频余弦，通过纯相位运算产生共振峰效果。");

    add(Language::Zh, "line_select",
        "线路选择 (1, 2, 1+2', 1+1')",
        "振荡器路由与复音数权衡",
        "选择音色使用 1 条振荡器线路（16 复音）还是叠加/失谐的 2 条线路（8 复音）。",
        "Line 1 或 2 = 16 复音，纯净单振荡器音色。Line 1+2' = 8 复音，厚实的合唱/失谐或分层音色。Line 1+1' = 自我双倍叠加。",
        "在双线模式 (1+2' 或 1+1') 下，每个音符会占用 CZ-3000 硬件上的两个 PD 通道，使最大复音数从 16 减半为 8。");

    add(Language::Zh, "detune",
        "失谐微调 (+/- Oct, Note, Fine)",
        "Line 1 与 Line 2' 之间的音高偏移",
        "相对于 Line 1，以八度、半音和音分微调 (Fine: 0-60) 改变第二条线路 (2' 或 1') 的音高。",
        "轻微的 Fine 微调 (02-08) 可营造模拟合唱般的厚重感；+12 或 +7 半音（五度）可做出风琴或强力和弦效果。",
        "向辅助振荡器通道添加固定的相位增量偏移。");

    add(Language::Zh, "vibrato",
        "颤音 (LFO)",
        "周期性音高调制",
        "在可选的延迟 (Delay) 之后，使用低频振荡器（波形 1-4：三角波、上锯齿、下锯齿、方波）调制音高。",
        "为弦乐/主音增添自然表现力，或用方波和高 Depth 制作科幻颤音与警笛音效。",
        "Delay (00-99) 控制起振延迟，Rate (00-99) 决定 LFO 速度，Depth (00-99) 决定调制深度。");

    add(Language::Zh, "key_follow",
        "键盘跟踪 (DCW 与 DCA 0-9)",
        "亮度与衰减随音区变化",
        "调整从低音区到高音区时，DCW 亮度与 DCA 包络速度的响应曲线。",
        "DCW 跟踪可抑制高音区的刺耳感并保持温暖；DCA 跟踪可使高音比低音衰减更快，如同真实钢琴或竖琴。",
        "根据 MIDI 音符编号按比例缩放 DCW 电平与 DCA 速率。");

    add(Language::Zh, "ring_noise",
        "环形调制与噪声调制",
        "金属钟声、锣声与气息感",
        "将 Line 1 与 Line 2' 进行环形调制 (Ring Mod)，或在相位中混入数字噪声调制 (Noise Mod)。",
        "Ring Mod 可生成非谐波的钟声、锣声和金属音色；Noise Mod 可添加军鼓瞬态、气息声或沙砾打击感。",
        "环形调制将两个振荡器输出相乘；噪声调制将伪随机移位寄存器注入相位/振幅路径。");

    add(Language::Zh, "sysex_io",
        "SysEx 与 CZ-3000 MIDI 音色库",
        "实时编辑缓冲区与内存传输",
        "通过 263/264 字节的 MIDI 系统专有信息 (SysEx) 与 Casio CZ-3000 直接通信。",
        "每次调节滑块都可实时更新 CZ-3000 编辑缓冲区，也可批量读取或发送 32 个预设、32 个用户记忆及 `.syx` 音色库文件。",
        "Casio CZ SysEx 将每个参数编码为 4 位半字节（低半字节在前），帧头为 `F0 44 00 00 7x ... F7`。");

    // =========================================================================
    // BRAZILIAN PORTUGUESE (PtBr)
    // =========================================================================
    add(Language::PtBr, "intro",
        "O que é Distorção de Fase (PD)?",
        "Síntese digital clássica da Casio (1984-1985)",
        "Diferente dos sintetizadores analógicos (VCO -> VCF -> VCA) ou da síntese FM da Yamaha, o Casio CZ gera som lendo uma onda cosseno da memória e distorcendo a velocidade com que o ângulo de fase é percorrido.",
        "Produz timbres digitais nítidos e encorpados, desde metais e baixos estilo analógico até sinos cristalinos e leads metálicos.",
        "Um contador digital de fase avança de 0 a 2*PI. Com DCW = 0 a leitura é linear (onda pura). Ao aumentar o DCW, a fase desacelera ou acelera no ciclo, gerando harmônicos brilhantes como ao abrir um filtro passa-baixa.");

    add(Language::PtBr, "dco",
        "DCO - Oscilador Controlado Digitalmente",
        "Controla a Afinação e seu Envelope",
        "O DCO define a frequência fundamental (afinação) da nota e permite modulá-la ao longo do tempo com um Envelope de Pitch de 8 estágios.",
        "Cria varreduras de afinação, efeitos laser, ligados expressivos de metais ou transientes acústicos naturais.",
        "Controla o incremento de passo do acumulador de fase. Modular o DCO altera a taxa de leitura da tabela de fase em semitons/cents nos 8 estágios.");

    add(Language::PtBr, "dcw",
        "DCW - Onda Controlada Digitalmente",
        "O 'Filtro' da Distorção de Fase",
        "O DCW controla quanta distorção é aplicada à fase da onda. Musicalmente funciona como um filtro passa-baixa ressonante (VCF) analógico.",
        "Nível 0 = onda senoidal pura e fechada. Níveis altos = som brilhante, agressivo, tipo dente de serra ou ressonante.",
        "Varia a não-linearidade do ângulo de leitura de fase. O envelope DCW de 8 passos molda o brilho ao longo do tempo.");

    add(Language::PtBr, "dca",
        "DCA - Amplificador Controlado Digitalmente",
        "Controla o Volume e a Dinâmica",
        "O DCA molda a amplitude (volume) total do som desde o toque da tecla até a soltura usando um envelope de 8 passos.",
        "Define se o som é um dedilhado percussivo, um pad de ataque lento ou um órgão sustentado.",
        "Multiplica a amostra final da onda pelo valor atual do envelope de amplitude de 8 estágios antes da saída.");

    add(Language::PtBr, "envelope",
        "Envelope CZ de 8 Estágios (Rate e Level)",
        "Além do ADSR tradicional",
        "Cada seção DCO, DCW e DCA tem até 8 estágios programáveis. Cada estágio define uma Velocidade (Rate: 00-99) para atingir um Nível (Level: 00-99).",
        "Permite crescendos complexos com múltiplos picos, ecos, formas rítmicas em degraus ou contornos ADSR clássicos.",
        "Rate 99 é instantâneo; Rate 00 é o mais lento. Level 99 é máximo; Level 00 é silêncio. Você pode definir Sustain e End Step entre os passos 1 e 8.");

    add(Language::PtBr, "sustain",
        "Ponto de Sustain (SUS)",
        "Onde a nota sustenta enquanto a tecla está pressionada",
        "Marca o passo onde o envelope pausa enquanto você segura a tecla. Ao soltar a tecla, o envelope avança para o próximo passo rumo ao End.",
        "Sem Sustain, o som percorre todo o envelope até o End independentemente de quanto tempo você segurar a tecla (ideal para percussão).",
        "Apenas um passo (1..8) pode ser marcado como Sustain, ou Nenhum. No CZ-3000, Sustain deve ser <= End Step.");

    add(Language::PtBr, "end_step",
        "Passo Final (END)",
        "Último estágio ativo do envelope",
        "Define qual estágio (1..8) encerra o envelope. Passos posteriores ao End são ignorados.",
        "Encurta estágios não utilizados para criar envelopes rápidos de 3 ou 4 passos estilo ADSR sem esperar pelos 8 passos.",
        "Quando o DCA atinge o passo End, a voz termina e é liberada para a polifonia.");

    add(Language::PtBr, "waves",
        "33 Formas de Onda PD (W1 e W2)",
        "8 Formas Base + Combinações Duplas",
        "Cada oscilador pode alternar entre Onda 1 (W1) e Onda 2 (W2) a cada ciclo, gerando 33 espectros harmônicos distintos.",
        "Saw (1) = metais; Square (2) = oco/madeiras; Pulse (3) = nasal; Double Sine (4) = oitava/órgão; Saw-Pulse (5) = cordas encorpadas; Resonance I/II/III (6-8) = varreduras ressonantes sem VCF!",
        "As 3 ondas Resonance multiplicam um cosseno de alta frequência dentro de uma janela triangular sincronizada à fundamental.");

    add(Language::PtBr, "line_select",
        "Seleção de Linha (1, 2, 1+2', 1+1')",
        "Roteamento de osciladores e polifonia",
        "Escolhe se o timbre usa 1 linha de oscilador (16 vozes de polifonia) ou 2 linhas empilhadas/desafinadas (8 vozes de polifonia).",
        "Line 1 ou 2 = 16 vozes, som limpo de um oscilador. Line 1+2' = 8 vozes, som encorpado com chorus/detune ou camadas. Line 1+1' = duplica a Linha 1.",
        "Nos modos de 2 linhas (1+2' ou 1+1'), cada nota aloca dois canais de voz PD no hardware do CZ-3000, reduzindo a polifonia máxima de 16 para 8.");

    add(Language::PtBr, "detune",
        "Detune (+/- Oct, Note, Fine)",
        "Deslocamento tonal entre Linha 1 e Linha 2'",
        "Desloca a afinação da segunda linha (2' ou 1') em relação à Linha 1 em Oitavas, Semitons e Ajuste Fino em cents (0-60).",
        "Um Detune fino sutil (02-08) traz peso analógico/chorus. +12 semitons ou +7 (quinta) criam intervalos de órgão ou power chords.",
        "Soma um deslocamento fixo de incremento de fase ao canal do oscilador secundário.");

    add(Language::PtBr, "vibrato",
        "Vibrato (LFO)",
        "Modulação cíclica de afinação",
        "Aplica uma oscilação de baixa frequência (Onda 1-4: Triangular, Serra Cima, Serra Baixo, Quadrada) à afinação após um Delay opcional.",
        "Traz expressão acústica natural a cordas/leads, ou trinados sci-fi e sirenes com onda Quadrada e Depth alto.",
        "Delay (00-99) aguarda antes de iniciar a modulação; Rate (00-99) ajusta a velocidade do LFO; Depth (00-99) a profundidade.");

    add(Language::PtBr, "key_follow",
        "Key Follow (DCW e DCA 0-9)",
        "Acompanhamento de teclado para brilho e decaimento",
        "Ajusta como o brilho do DCW e a velocidade do envelope DCA respondem ao longo do teclado, dos graves aos agudos.",
        "DCW Key Follow suaviza os agudos para evitar aspereza e manter calor. DCA Key Follow faz notas agudas decaírem mais rápido, como num piano real.",
        "Escala o nível DCW e a taxa DCA proporcionalmente ao número da nota MIDI.");

    add(Language::PtBr, "ring_noise",
        "Modulação Ring e Noise",
        "Sinos metálicos, percussão e sopro",
        "Modula em anel a Linha 1 com a Linha 2' (Ring Mod) ou mistura modulação de ruído digital (Noise Mod) na fase.",
        "Ring Mod cria sinos inarmônicos e gongos. Noise Mod adiciona ataque de caixa, sopro ou texturas percussivas.",
        "Ring Modulation multiplica as saídas dos dois osciladores; Noise Modulation injeta um registrador pseudoaleatório no caminho de fase/amplitude.");

    add(Language::PtBr, "sysex_io",
        "SysEx e Banco MIDI CZ-3000",
        "Edit Buffer ao vivo e transferência de memórias",
        "Comunica diretamente com o Casio CZ-3000 via mensagens MIDI System Exclusive (SysEx) de 263/264 bytes por patch.",
        "Cada ajuste pode atualizar o Edit Buffer do CZ-3000 ao vivo, ou ler/enviar os 32 Presets, 32 Memórias de usuário e arquivos `.syx`.",
        "O SysEx do Casio CZ codifica cada parâmetro em nibbles de 4 bits (nibble baixo primeiro) com cabeçalho `F0 44 00 00 7x ... F7`.");

    // =========================================================================
    // FRENCH (Fr)
    // =========================================================================
    add(Language::Fr, "intro",
        "Qu'est-ce que la Distorsion de Phase (PD) ?",
        "Synthèse numérique classique Casio (1984-1985)",
        "Contrairement aux synthés analogiques (VCO -> VCF -> VCA) ou à la FM de Yamaha, le Casio CZ génère le son en lisant une onde cosinus en mémoire tout en distordant la vitesse de parcours de son angle de phase.",
        "Produit des timbres numériques nets et percutants, des cuivres et basses analogiques jusqu'aux cloches cristallines et leads métalliques.",
        "Un compteur numérique de phase balaie de 0 à 2*PI. À DCW = 0 la lecture est linéaire (sinusoïde pure). En augmentant DCW, la phase ralentit ou accélère en cours de cycle, ajoutant des harmoniques comme l'ouverture d'un filtre passe-bas.");

    add(Language::Fr, "dco",
        "DCO - Oscillateur Contrôlé Numériquement",
        "Contrôle la Hauteur et son Enveloppe",
        "Le DCO définit la fréquence fondamentale (hauteur) de la note et permet de la moduler dans le temps via une enveloppe de hauteur à 8 étages.",
        "Crée des balayages de hauteur, effets laser, attaques expressives de cuivres ou transitoires acoustiques naturels.",
        "Contrôle l'incrément de pas de l'accumulateur de phase. Moduler le DCO fait varier la vitesse de lecture de la table de phase sur 8 étapes.");

    add(Language::Fr, "dcw",
        "DCW - Onde Contrôlée Numériquement",
        "Le « Filtre » de la Distorsion de Phase",
        "Le DCW règle le taux de distorsion appliqué à la phase de l'onde. Il se comporte musicalement comme un filtre passe-bas résonant (VCF).",
        "Niveau 0 = onde sinusoïdale pure et sourde. Niveaux élevés = son brillant, mordant, type dent de scie ou résonant.",
        "Fait varier la non-linéarité de l'angle de lecture de phase. L'enveloppe DCW à 8 étages sculpte la brillance dans le temps.");

    add(Language::Fr, "dca",
        "DCA - Amplificateur Contrôlé Numériquement",
        "Contrôle le Volume et la Dynamique",
        "Le DCA sculpte l'amplitude (volume) globale du son de l'appui sur la touche jusqu'au relâchement via une enveloppe à 8 étages.",
        "Détermine si le timbre est un pincement percussif, une nappe à attaque lente ou un orgue soutenu.",
        "Multiplie l'échantillon final de l'onde par la valeur courante de l'enveloppe d'amplitude à 8 étages avant le mixage.");

    add(Language::Fr, "envelope",
        "Enveloppe CZ à 8 Étages (Rate & Level)",
        "Au-delà de l'ADSR classique",
        "Chaque section DCO, DCW et DCA dispose de 8 étages programmables. Chaque étage définit une Vitesse (Rate : 00-99) pour atteindre un Niveau (Level : 00-99).",
        "Permet des évolutions complexes multi-pics, des échos rebondissants, des motifs rythmiques en paliers ou des contours ADSR classiques.",
        "Rate 99 est instantané ; Rate 00 est le plus lent. Level 99 est le maximum ; Level 00 est le silence. Sustain et End Step se placent librement entre 1 et 8.");

    add(Language::Fr, "sustain",
        "Point de Sustain (SUS)",
        "Maintien tant que la touche est enfoncée",
        "Indique l'étape où l'enveloppe se met en pause tant que vous maintenez la touche. Au relâchement, elle passe à l'étape suivante vers End.",
        "Sans Sustain, le son parcourt toute l'enveloppe jusqu'à End quelle que soit la durée d'appui (idéal pour les percussions).",
        "Une seule étape (1..8) peut être définie comme Sustain, ou Aucune. Sur le CZ-3000, Sustain doit être <= End Step.");

    add(Language::Fr, "end_step",
        "Étape Finale (END)",
        "Dernier étage actif de l'enveloppe",
        "Définit quelle étape (1..8) termine l'enveloppe. Les étapes suivantes sont ignorées.",
        "Raccourcit les étapes inutilisées pour créer des enveloppes rapides à 3 ou 4 étapes type ADSR sans attendre 8 étapes.",
        "Lorsque le DCA atteint l'étape End, la voix se termine et est libérée pour la polyphonie.");

    add(Language::Fr, "waves",
        "33 Formes d'Onde PD (W1 & W2)",
        "8 Formes de Base + Combinaisons Duales",
        "Chaque oscillateur peut alterner entre Onde 1 (W1) et Onde 2 (W2) à chaque cycle, offrant 33 spectres harmoniques distincts.",
        "Saw (1) = cuivres ; Square (2) = bois/creux ; Pulse (3) = nasal ; Double Sine (4) = octave/orgue ; Saw-Pulse (5) = cordes riches ; Resonance I/II/III (6-8) = balayages résonants sans VCF !",
        "Les 3 ondes Resonance multiplient un cosinus haute fréquence dans une fenêtre triangulaire synchronisée à la fondamentale.");

    add(Language::Fr, "line_select",
        "Sélection de Ligne (1, 2, 1+2', 1+1')",
        "Routage des oscillateurs et polyphonie",
        "Choisit si le patch utilise 1 ligne d'oscillateur (16 voix de polyphonie) ou 2 lignes empilées/désaccordées (8 voix de polyphonie).",
        "Line 1 ou 2 = 16 voix, son pur à un oscillateur. Line 1+2' = 8 voix, son épais avec chorus/detune ou couches. Line 1+1' = double la Ligne 1.",
        "En mode 2 lignes (1+2' ou 1+1'), chaque note mobilise deux canaux PD sur le CZ-3000, réduisant la polyphonie de 16 à 8 voix.");

    add(Language::Fr, "detune",
        "Detune (+/- Oct, Note, Fine)",
        "Décalage tonal entre Ligne 1 et Ligne 2'",
        "Décale la hauteur de la seconde ligne (2' ou 1') par rapport à la Ligne 1 en Octaves, Demi-tons et Centièmes Fine (0-60).",
        "Un léger Detune fin (02-08) apporte une épaisseur analogique/chorus. +12 demi-tons ou +7 (quinte) créent des intervalles d'orgue ou power-chords.",
        "Ajoute un décalage fixe d'incrément de phase au canal de l'oscillateur secondaire.");

    add(Language::Fr, "vibrato",
        "Vibrato (LFO)",
        "Modulation cyclique de la hauteur",
        "Applique une oscillation basse fréquence (Onde 1-4 : Triangle, Scie montante, Scie descendante, Carrée) à la hauteur après un Delay optionnel.",
        "Apporte une expressivité naturelle aux cordes/leads, ou des trilles sci-fi et sirènes avec l'onde Carrée et un Depth élevé.",
        "Delay (00-99) retarde l'entrée de la modulation ; Rate (00-99) règle la vitesse du LFO ; Depth (00-99) règle l'amplitude.");

    add(Language::Fr, "key_follow",
        "Key Follow (DCW & DCA 0-9)",
        "Suivi de clavier pour brillance et déclin",
        "Ajuste la façon dont la brillance DCW et la vitesse DCA réagissent sur l'étendue du clavier, des graves aux aigus.",
        "DCW Key Follow adoucit les aigus pour éviter l'agressivité. DCA Key Follow raccourcit le déclin des notes aiguës comme sur un vrai piano.",
        "Met à l'échelle le niveau DCW et la vitesse DCA proportionnellement au numéro de note MIDI.");

    add(Language::Fr, "ring_noise",
        "Modulation Ring & Noise",
        "Cloches métalliques, gong et souffle",
        "Module en anneau la Ligne 1 et la Ligne 2' (Ring Mod) ou injecte une modulation de bruit numérique (Noise Mod) dans la phase.",
        "Ring Mod crée des cloches inharmoniques et gongs. Noise Mod ajoute un claquant de caisse claire, du souffle ou des textures percussives.",
        "Ring Modulation multiplie les sorties des deux oscillateurs ; Noise Modulation injecte un registre pseudo-aléatoire dans le chemin phase/amplitude.");

    add(Language::Fr, "sysex_io",
        "SysEx & Banque MIDI CZ-3000",
        "Edit Buffer en direct & transfert mémoire",
        "Communique directement avec le Casio CZ-3000 via des messages MIDI System Exclusive (SysEx) de 263/264 octets par patch.",
        "Chaque réglage met à jour l'Edit Buffer du CZ-3000 en direct, ou lit/envoie les 32 Presets, 32 Mémoires utilisateur et fichiers `.syx`.",
        "Le SysEx Casio CZ encode chaque paramètre en nibbles de 4 bits (nibble bas d'abord) avec l'en-tête `F0 44 00 00 7x ... F7`.");
}

void CzTutorial::tooltip(const char* topic_id) {
    if (!enabled_) return;

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_Stationary)) {
        const auto& topics = current_topics();
        auto it = topics.find(topic_id);
        if (it != topics.end()) {
            const auto& t = it->second;
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 32.0f);

            // Header banner
            ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "[%s] %s", tr("tut.badge"), t.title.c_str());
            ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), "%s", t.subtitle.c_str());
            ImGui::Separator();
            ImGui::Spacing();

            // Summary
            ImGui::TextUnformatted(t.summary.c_str());
            ImGui::Spacing();

            // Sonic Effect
            ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.20f, 1.0f), "%s", tr("tut.effect"));
            ImGui::TextUnformatted(t.sonic_effect.c_str());
            ImGui::Spacing();

            // PD Engine Detail
            ImGui::TextColored(ImVec4(0.40f, 0.90f, 0.40f, 1.0f), "%s", tr("tut.tech"));
            ImGui::TextUnformatted(t.pd_explanation.c_str());

            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}

void CzTutorial::render_guide_window(bool* p_open) {
    ImGui::SetNextWindowSize(ImVec2(720, 480), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(tr("tut.win_title"), p_open)) {
        ImGui::End();
        return;
    }

    const auto& topics = current_topics();

    // Left sidebar: topic selector
    ImGui::BeginChild("TutTopics", ImVec2(220, 0), true);
    ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "%s", tr("tut.topics"));
    ImGui::Separator();

    for (const auto& id : topic_order_) {
        auto it = topics.find(id);
        if (it == topics.end()) continue;
        bool selected = (active_guide_topic_ == id);
        if (ImGui::Selectable(it->second.title.c_str(), selected)) {
            active_guide_topic_ = id;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right pane: deep explanation
    ImGui::BeginChild("TutContent", ImVec2(0, 0), true);
    auto it = topics.find(active_guide_topic_);
    if (it != topics.end()) {
        const auto& t = it->second;
        ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "%s", t.title.c_str());
        ImGui::TextDisabled("%s", t.subtitle.c_str());
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::TextWrapped("%s", t.summary.c_str());
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.20f, 1.0f), "%s", tr("tut.effect"));
        ImGui::TextWrapped("%s", t.sonic_effect.c_str());
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::TextColored(ImVec4(0.40f, 0.90f, 0.40f, 1.0f), "%s", tr("tut.tech"));
        ImGui::TextWrapped("%s", t.pd_explanation.c_str());
    }
    ImGui::EndChild();

    ImGui::End();
}

} // namespace cz
