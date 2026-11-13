#include "cz_tutorial.h"

namespace cz {

CzTutorial& CzTutorial::instance() {
    static CzTutorial s_instance;
    return s_instance;
}

CzTutorial::CzTutorial() {
    init_topics();
}

void CzTutorial::init_topics() {
    topics_["intro"] = {
        "¿Qué es la Distorsión de Fase (Phase Distortion)?",
        "La innovadora síntesis de Casio frente al sonido analógico tradicional",
        "En los sintetizadores analógicos comunes (Moog, Juno, Prophet), se usa 'Síntesis Sustractiva': se arranca con una onda zumbona y áspera (sierra o pulso) y se usa un Filtro Analógico (VCF) para cortar agudos.\n\n"
        "¡En el Casio CZ NO HAY FILTRO ANALÓGICO! En su lugar, lee una onda senoidal suave y pura grabada en memoria, pero deforma ('distorsiona') la velocidad de lectura del ángulo de fase en cada ciclo.",
        "Pasa de un tono suave y apagado a un sonido brillante, afilado y rico en armónicos sin desafinar ni perder calidez.",
        "El control DCW (Digitally Controlled Waveform) es quien hace de 'filtro'. A nivel 0 la onda es un seno puro. Al subirlo a 99, la fase se acelera bruscamente, transformando la onda en diente de sierra, cuadrada o resonante."
    };

    topics_["dco"] = {
        "DCO: Digitally Controlled Oscillator",
        "Controlador Digital de Tono y Envolvente de Frecuencia",
        "El DCO define la nota base que estás tocando y cómo cambia la afinación a lo largo del tiempo.",
        "Permite crear caídas de tono rápidas para golpes de percusión y bombos, barridos de tono para efectos de ciencia ficción o sutiles deslices como los de un cantante o instrumento de cuerda.",
        "El CZ calcula la frecuencia base y la modula mediante una envolvente de 8 pasos con valores de velocidad (Rate) y nivel (Level)."
    };

    topics_["dcw"] = {
        "DCW: Digitally Controlled Waveform",
        "El 'Filtro' del CZ (Control de Brillo y Armónicos)",
        "¡Este es el parámetro más importante para moldear el timbre en un Casio CZ! Controla cuánta Distorsión de Fase se aplica.",
        "A nivel 0, el sonido es completamente redondo, oscuro y suave (seno puro). A medida que sube hacia 99, el sonido se llena de armónicos cortantes, brillantes y enérgicos, emulando la apertura del filtro cutoff analógico.",
        "Modifica el ángulo de lectura de la fase digital. Al modularlo con su envolvente de 8 pasos, recrea el típico barrido de filtro 'wah', metales que brillan al soplar o bajos que chasquean con fuerza al pulsar la nota."
    };

    topics_["dca"] = {
        "DCA: Digitally Controlled Amplifier",
        "Control de Volumen y Dinámica Temporal",
        "El DCA determina la sonoridad o volumen del instrumento a medida que transcurre el tiempo.",
        "Define si el sonido explota de inmediato (como un piano o tambor), si entra lentamente con suavidad (como una sección de cuerdas o pad ambiental) y cuánto tarda en extinguirse tras soltar la tecla.",
        "Un convertidor digital multiplica la señal por la envolvente de amplitud de 8 pasos. El nivel 0 es silencio total y 99 es volumen máximo."
    };

    topics_["envelope"] = {
        "Envolventes Casio de 8 Pasos",
        "Mucho más expresivas que el clásico ADSR de 4 etapas",
        "Casi todos los sintetizadores tienen 4 etapas: Attack, Decay, Sustain, Release (ADSR). ¡El CZ te ofrece 8 pasos independientes!",
        "Permite crear evoluciones complejas: un doble ataque (como el arco frotando dos veces una cuerda), ecos rítmicos, modulaciones en escalera o caídas escalonadas.",
        "Cada paso tiene dos controles: RATE (la velocidad de transición, 0=lento, 99=instantáneo) y LEVEL (el valor de destino, 0 a 99). Además cuenta con puntos clave: SUSTAIN (donde reposa la nota mientras mantienes presionada la tecla) y END (donde termina el sonido)."
    };

    topics_["sustain"] = {
        "Punto SUSTAIN (Sostenido)",
        "Punto de mantenimiento mientras la tecla esté pulsada",
        "Indica en cuál de los 8 pasos la envolvente se 'congela' y sostiene el sonido mientras tengas el dedo puesto en la tecla.",
        "Al soltar la tecla, el sintetizador continúa automáticamente desde el paso de Sustain hacia los siguientes pasos hasta llegar al final (END), actuando como la fase de Release.",
        "En el formato SysEx del CZ, el bit más significativo (0x80) del byte de nivel correspondiente a este paso se pone a 1 para avisarle al microcontrolador de la máquina."
    };

    topics_["end_step"] = {
        "Punto END (Fin de Envolvente)",
        "Finalización del ciclo sonoro",
        "Define el último paso activo de la envolvente. Una vez alcanzado este paso, el ciclo termina y el oscilador se apaga.",
        "Cualquier paso posterior al número marcado como END es ignorado por el sintetizador, ahorrando tiempo de procesamiento.",
        "Permite crear sonidos cortos y ágiles de solo 2 o 3 pasos (por ejemplo, bajos directos o percusiones) sin tener que configurar los 8 pasos."
    };

    topics_["waves"] = {
        "Formas de Onda (Waveforms 1 a 8)",
        "Las 8 texturas de Distorsión de Fase fundamentales",
        "El CZ ofrece 8 formas de onda:\n"
        "1. Diente de Sierra (Saw): Cuerdas, metales y leads de sintetizador clásicos.\n"
        "2. Cuadrada (Square): Sonido hueco y maderoso, ideal para clarinetes y bajos funk.\n"
        "3. Pulso (Pulse): Sonido afilado y estrecho, perfecto para oboes y timbres cortantes.\n"
        "4. Doble Seno: Campanas, pianos eléctricos brillantes y timbres metálicos.\n"
        "5. Sierra-Pulso: Híbrido denso para bajos gruesos con presencia.\n"
        "6, 7, 8. Resonantes 1, 2 y 3: ¡Emulan resonancias de filtro analógico con picos dinámicos!",
        "Las ondas resonantes permiten barridos con auto-oscilación simulada sin necesidad de circuitos analógicos inestables.",
        "Al activar 'USE WAVE 2', el sintetizador alterna entre la Onda 1 y la Onda 2 en ciclos alternos, creando híbridos timbrales imposibles de obtener en otros sintetizadores."
    };

    topics_["line_select"] = {
        "Line Select (Estructura de Voces: Línea 1 y Línea 2)",
        "Gestión de los 16 osciladores del CZ-3000",
        "El CZ-3000 contiene dos sintetizadores completos en su interior: Línea 1 (DCO1+DCW1+DCA1) y Línea 2 (DCO2+DCW2+DCA2).\n"
        "• '1': Solo suena la Línea 1 (8 voces de polifonía completa).\n"
        "• '2': Solo suena la Línea 2 (8 voces de polifonía completa).\n"
        "• '1+1'': La Línea 1 se duplica a sí misma con desafinación estéreo (4 voces, sonido gigantesco estilo chorus/unison).\n"
        "• '1+2'': Mezcla dos instrumentos distintos simultáneamente (ej: campana percusiva en Línea 1 y colchón de cuerdas en Línea 2).",
        "Con '1+1'' o '1+2'' obtienes un sonido acústico rico y espacioso, simulando conjuntos orquestales y sintetizadores analógicos polifónicos pesados.",
        "Controla internamente los bits de enrutamiento del bus de voz en el chip de generación sonora de Casio."
    };

    topics_["detune"] = {
        "DETUNE: Desafinación entre Líneas",
        "Afinación relativa para grosor, chorus y armonía",
        "Permite desplazar la afinación de la Línea 2 respecto a la Línea 1 con tres controles: Octava (0-3), Nota musical (0-11 semitonos) y Ajuste Fino (Fine: 0-60).",
        "Un valor pequeño de 'Fine' (+2 o +3) produce un batimiento de frecuencias que genera un efecto de Chorus natural muy cálido. Un intervalo de quinta (+7 notas) o una octava crea acordes y leads dobles impresionantes.",
        "El CZ permite seleccionar polaridad POSITIVA o NEGATIVA ('Sense') para decidir si la Línea 2 desafina hacia arriba o hacia abajo."
    };

    topics_["vibrato"] = {
        "VIBRATO: Modulación LFO de Tono",
        "Ondulación expresiva y orgánica",
        "Añade una oscilación periódica al tono de las notas para transmitir expresividad musical.\n"
        "• WAVE: Forma de modulación (1=Triangular, 2=Sierra ascendente, 3=Sierra descendente, 4=Cuadrada).\n"
        "• DELAY: Tiempo en silencio antes de que comience el vibrato tras pulsar la tecla.\n"
        "• RATE: Frecuencia o rapidez con que oscila.\n"
        "• DEPTH: Amplitud o intensidad de la desafinación.",
        "Ideal para imitar el vibrato de violines, voces humanas o trompetas, que no vibran al instante del ataque sino tras mantener la nota unos milisegundos.",
        "El LFO digital modula directamente los registros de fase del acumulador DCO."
    };

    topics_["key_follow"] = {
        "KEY FOLLOW (Seguimiento de Teclado)",
        "Respuesta dinámica según la altura de la nota tocada",
        "En los instrumentos acústicos reales (como el piano de cola o el violonchelo), las cuerdas agudas son cortas y brillantes, mientras que las notas graves resuenan mucho más tiempo y con armónicos más oscuros.",
        "El Key Follow hace que al tocar notas más altas en el teclado, las envolventes avancen más rápido (haciendo el sonido más ágil y percusivo) o ajusta el brillo del DCW.",
        "Evita que las notas agudas suenen apagadas o demasiado largas, dando una respuesta equilibrada a lo largo de todo el teclado de 61 teclas."
    };

    topics_["ring_noise"] = {
        "Modulación en Anillo (Ring Mod) y Ruido (Noise)",
        "Efectos inarmónicos, percusión y timbres metálicos",
        "• RING MODULATION: Multiplica matemáticamente la forma de onda de la Línea 1 por la Línea 2. Genera frecuencias sumadas y restadas que no tienen relación armónica simple, produciendo campanas de iglesia, gongs de metal, ruidos extraterrestres y voces robóticas.\n"
        "• NOISE MODULATION: Modula la ventana de lectura con ruido seudo-aleatorio para simular soplidos de flauta, viento, olas de mar o el impacto arenoso de cajas de batería.",
        "Añade texturas de sonido experimental o componentes de ataque percusivo muy realistas.",
        "Altera los bits de modulación en la palabra MFW de la Línea 1 en el protocolo SysEx de Casio."
    };

    topics_["sysex_io"] = {
        "Envío y Recepción SysEx (System Exclusive)",
        "Comunicación directa con tu sintetizador CZ-3000 real",
        "SysEx es el protocolo MIDI especializado que permite transferir datos de memoria interna y parámetros de sonido entre una computadora y el hardware.",
        "• 'ENVIAR AL BÚFER': Envía el sonido actual a la memoria de edición activa del CZ-3000 para que lo toques y escuches al instante sin sobrescribir nada.\n"
        "• 'ESCRIBIR EN BANCO': Guarda el sonido de forma permanente en la ranura de memoria seleccionada (A1 a D8).\n"
        "• 'RECIBIR DEL CZ': Pide al sintetizador que transmita su memoria para cargarla en la aplicación.",
        "Utiliza la trama oficial de Casio: 0xF0 0x44 0x00 0x00 [CANAL] [TIPO] [SLOT] [DATOS_NIBBLES] 0xF7 con verificación de longitud exacta (264 bytes)."
    };
}

void CzTutorial::tooltip(const char* topic_id) {
    if (!enabled_) return;

    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_Stationary)) {
        auto it = topics_.find(topic_id);
        if (it != topics_.end()) {
            const auto& t = it->second;
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 32.0f);

            // Header banner
            ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "[MODO TUTORIAL] %s", t.title.c_str());
            ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), "%s", t.subtitle.c_str());
            ImGui::Separator();

            // Summary
            ImGui::Spacing();
            ImGui::TextUnformatted(t.summary.c_str());

            // Sonic effect
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.20f, 1.0f), "Efecto en el Sonido:");
            ImGui::TextUnformatted(t.sonic_effect.c_str());

            // PD Secret
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(0.40f, 0.90f, 0.40f, 1.0f), "Cómo Funciona la Distorsión de Fase (PD):");
            ImGui::TextUnformatted(t.pd_explanation.c_str());

            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}

void CzTutorial::render_guide_window(bool* p_open) {
    if (!p_open || !*p_open) return;

    ImGui::SetNextWindowSize(ImVec2(620, 520), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Guia Didactica: Distorsion de Fase (Casio CZ)", p_open)) {
        // Top switch
        bool en = enabled_;
        if (ImGui::Checkbox("Activar Tooltips Interactivos al pasar el mouse por los botones", &en)) {
            enabled_ = en;
        }
        ImGui::Separator();

        // Left topics list, right content
        ImGui::Columns(2, "GuideColumns", true);
        ImGui::SetColumnWidth(0, 180);

        ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "TEMAS:");
        ImGui::Separator();

        for (const auto& [id, t] : topics_) {
            bool selected = (active_guide_topic_ == id);
            if (ImGui::Selectable(t.title.c_str(), selected)) {
                active_guide_topic_ = id;
            }
        }

        ImGui::NextColumn();

        auto it = topics_.find(active_guide_topic_);
        if (it != topics_.end()) {
            const auto& t = it->second;
            ImGui::TextColored(ImVec4(0.20f, 0.90f, 0.85f, 1.0f), "%s", t.title.c_str());
            ImGui::TextColored(ImVec4(0.70f, 0.70f, 0.70f, 1.0f), "%s", t.subtitle.c_str());
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::TextWrapped("%s", t.summary.c_str());
            ImGui::Spacing();
            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.20f, 1.0f), "¿Qué cambia en el sonido al tocar este parámetro?");
            ImGui::TextWrapped("%s", t.sonic_effect.c_str());
            ImGui::Spacing();

            ImGui::TextColored(ImVec4(0.40f, 0.90f, 0.40f, 1.0f), "Física de la Distorsión de Fase (PD):");
            ImGui::TextWrapped("%s", t.pd_explanation.c_str());
        }

        ImGui::Columns(1);
    }
    ImGui::End();
}

} // namespace cz

