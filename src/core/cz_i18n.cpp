#include "cz_i18n.h"
#include <windows.h>
#include <fstream>

namespace cz {

CzI18n& CzI18n::instance() {
    static CzI18n s_instance;
    return s_instance;
}

CzI18n::CzI18n() {
    build_tables();
    load_saved_language();
}

void CzI18n::set_language(Language lang) {
    lang_ = lang;
    save_language();
}

const char* CzI18n::tr(const char* key) const {
    size_t idx = static_cast<size_t>(lang_);
    if (idx < 8) {
        auto it = tables_[idx].find(key);
        if (it != tables_[idx].end()) {
            return it->second.c_str();
        }
    }
    auto en_it = tables_[0].find(key);
    if (en_it != tables_[0].end()) {
        return en_it->second.c_str();
    }
    return key;
}

const char* CzI18n::language_name(Language lang) {
    switch (lang) {
    case Language::En:   return "English (Default)";
    case Language::Es:   return "Español";
    case Language::It:   return "Italiano";
    case Language::De:   return "Deutsch";
    case Language::Ja:   return "日本語 (Japanese)";
    case Language::Zh:   return "简体中文 (Chinese)";
    case Language::PtBr: return "Português (Brasil)";
    case Language::Fr:   return "Français";
    }
    return "English";
}

const char* CzI18n::language_code(Language lang) {
    switch (lang) {
    case Language::En:   return "en";
    case Language::Es:   return "es";
    case Language::It:   return "it";
    case Language::De:   return "de";
    case Language::Ja:   return "ja";
    case Language::Zh:   return "zh";
    case Language::PtBr: return "pt_BR";
    case Language::Fr:   return "fr";
    }
    return "en";
}

std::string CzI18n::get_config_path() const {
    char local_app_data[MAX_PATH] = {0};
    DWORD len = GetEnvironmentVariableA("LOCALAPPDATA", local_app_data, MAX_PATH);
    if (len == 0 || len >= MAX_PATH) {
        return "lang.cfg";
    }
    std::string dir = std::string(local_app_data) + "\\CZ3000SysEx";
    CreateDirectoryA(dir.c_str(), nullptr);
    return dir + "\\lang.cfg";
}

void CzI18n::load_saved_language() {
    std::ifstream f(get_config_path());
    if (!f.is_open()) {
        lang_ = Language::En;
        return;
    }
    std::string code;
    std::getline(f, code);
    for (int i = 0; i < 8; ++i) {
        Language l = static_cast<Language>(i);
        if (code == language_code(l)) {
            lang_ = l;
            return;
        }
    }
    lang_ = Language::En;
}

void CzI18n::save_language() const {
    std::ofstream f(get_config_path());
    if (f.is_open()) {
        f << language_code(lang_) << "\n";
    }
}

void CzI18n::build_tables() {
    auto add = [&](const char* key,
                   const char* en, const char* es, const char* it, const char* de,
                   const char* ja, const char* zh, const char* ptbr, const char* fr) {
        tables_[0][key] = en;
        tables_[1][key] = es;
        tables_[2][key] = it;
        tables_[3][key] = de;
        tables_[4][key] = ja;
        tables_[5][key] = zh;
        tables_[6][key] = ptbr;
        tables_[7][key] = fr;
    };

    // Main Menu
    add("menu.file", "File", "Archivo", "File", "Datei", "ファイル", "文件", "Arquivo", "Fichier");
    add("menu.load_syx", "Load SysEx File (.syx)...", "Cargar Archivo SysEx (.syx)...", "Carica File SysEx (.syx)...", "SysEx-Datei laden (.syx)...", "SysExファイルを開く (.syx)...", "加载 SysEx 文件 (.syx)...", "Carregar Arquivo SysEx (.syx)...", "Charger un fichier SysEx (.syx)...");
    add("menu.save_syx", "Save SysEx Bank (.syx)...", "Guardar Banco SysEx (.syx)...", "Salva Banco SysEx (.syx)...", "SysEx-Bank speichern (.syx)...", "SysExバンクを保存 (.syx)...", "保存 SysEx 音色库 (.syx)...", "Salvar Banco SysEx (.syx)...", "Enregistrer la banque SysEx (.syx)...");
    add("menu.reset_bank", "Reset Factory Bank", "Restablecer Banco de Fábrica", "Ripristina Banco di Fabbrica", "Werkseinstellungen laden", "ファクトリーバンクに戻す", "恢复出厂音色库", "Restaurar Banco de Fábrica", "Réinitialiser la banque d'usine");

    add("menu.views", "Views", "Vistas", "Viste", "Ansichten", "表示", "视图", "Visualizações", "Affichage");
    add("menu.env_graph", "Envelope Graph", "Gráfico de Envolventes", "Grafico Inviluppi", "Hüllkurvengrafik", "エンベロープグラフ", "包络线图表", "Gráfico de Envelopes", "Graphique d'enveloppes");
    add("menu.librarian", "Bank Librarian", "Gestor de Biblioteca (Librarian)", "Gestore Banco (Librarian)", "Bank-Manager (Librarian)", "バンクライブラリアン", "音色库管理器", "Gerenciador de Banco", "Gestionnaire de banque");
    add("menu.pd_guide", "Phase Distortion Guide", "Guía Didáctica de Distorsión de Fase", "Guida alla Distorsione di Fase", "Phase-Distortion-Handbuch", "フェーズディストーションガイド", "相位失真指南", "Guia de Distorção de Fase", "Guide de distorsion de phase");
    add("menu.manual", "CZ-3000 Operation Manual", "Manual Oficial CZ-3000", "Manuale Ufficiale CZ-3000", "CZ-3000 Bedienungsanleitung", "CZ-3000 取扱説明書", "CZ-3000 官方操作手册", "Manual Oficial CZ-3000", "Manuel officiel CZ-3000");

    add("menu.midi", "MIDI", "MIDI", "MIDI", "MIDI", "MIDI", "MIDI", "MIDI", "MIDI");
    add("menu.midi_in", "MIDI Input Port (In):", "Puerto de Entrada (In):", "Porta Ingresso MIDI (In):", "MIDI-Eingang (In):", "MIDI入力ポート (In):", "MIDI 输入端口 (In):", "Porta de Entrada MIDI (In):", "Port d'entrée MIDI (In) :");
    add("menu.no_midi_in", "  (No MIDI In devices)", "  (Sin dispositivos MIDI In)", "  (Nessun dispositivo MIDI In)", "  (Keine MIDI-In-Geräte)", "  (MIDI入力デバイスなし)", "  (无 MIDI 输入设备)", "  (Sem dispositivos MIDI In)", "  (Aucun périphérique MIDI In)");
    add("menu.midi_out", "MIDI Output Port (Out):", "Puerto de Salida (Out):", "Porta Uscita MIDI (Out):", "MIDI-Ausgang (Out):", "MIDI出力ポート (Out):", "MIDI 输出端口 (Out):", "Porta de Saída MIDI (Out):", "Port de sortie MIDI (Out) :");
    add("menu.no_midi_out", "  (No MIDI Out devices)", "  (Sin dispositivos MIDI Out)", "  (Nessun dispositivo MIDI Out)", "  (Keine MIDI-Out-Geräte)", "  (MIDI出力デバイスなし)", "  (无 MIDI 输出设备)", "  (Sem dispositivos MIDI Out)", "  (Aucun périphérique MIDI Out)");
    add("menu.midi_ch", "Basic MIDI Channel (CH):", "Canal MIDI Básico (CH):", "Canale MIDI Base (CH):", "MIDI-Basiskanal (CH):", "基本MIDIチャンネル (CH):", "基本 MIDI 通道 (CH):", "Canal MIDI Básico (CH):", "Canal MIDI de base (CH) :");
    add("menu.select_ch", "Select Channel (1-16)", "Seleccionar Canal (1-16)", "Seleziona Canale (1-16)", "Kanal wählen (1-16)", "チャンネル選択 (1-16)", "选择通道 (1-16)", "Selecionar Canal (1-16)", "Choisir le canal (1-16)");
    add("menu.ch_prefix", "Channel %02d", "Canal %02d", "Canale %02d", "Kanal %02d", "チャンネル %02d", "通道 %02d", "Canal %02d", "Canal %02d");
    add("menu.cz_prg_hint", "  On CZ-3000: MIDI button -> PRG=ENA", "  En el CZ-3000: botón MIDI -> PRG=ENA", "  Sul CZ-3000: tasto MIDI -> PRG=ENA", "  Am CZ-3000: MIDI-Taste -> PRG=ENA", "  CZ-3000本体: MIDIボタン -> PRG=ENA", "  在 CZ-3000 上: MIDI 按钮 -> PRG=ENA", "  No CZ-3000: botão MIDI -> PRG=ENA", "  Sur le CZ-3000 : bouton MIDI -> PRG=ENA");

    add("menu.language", "Language", "Idioma", "Lingua", "Sprache", "言語", "语言", "Idioma", "Langue");
    add("btn.tutorial_on", "[?] TUTORIAL MODE: ON", "[?] MODO TUTORIAL: ON", "[?] MODO TUTORIAL: ON", "[?] TUTORIAL-MODUS: AN", "[?] チュートリアル: ON", "[?] 教程模式: 开", "[?] MODO TUTORIAL: ON", "[?] MODE TUTORIEL : ON");
    add("btn.tutorial_off", "[?] TUTORIAL MODE: OFF", "[?] MODO TUTORIAL: OFF", "[?] MODO TUTORIAL: OFF", "[?] TUTORIAL-MODUS: AUS", "[?] チュートリアル: OFF", "[?] 教程模式: 关", "[?] MODO TUTORIAL: OFF", "[?] MODE TUTORIEL : OFF");
    add("btn.guide", "Guide", "Guía", "Guida", "Hilfe", "ガイド", "指南", "Guia", "Guide");

    // Keyboard & Status Bar
    add("kbd.title", "LIVE AUDITION KEYBOARD (6 OCTAVES: C1 - C7):", "TECLADO DE AUDICION EN VIVO (6 OCTAVAS: C1 - C7):", "TASTIERA D'ASCOLTO LIVE (6 OTTAVE: C1 - C7):", "LIVE-TESTTASTATUR (6 OKTAVEN: C1 - C7):", "ライブ試聴キーボード (6オクターブ: C1 - C7):", "实时试听键盘 (6 个八度: C1 - C7):", "TECLADO DE AUDIÇÃO AO VIVO (6 OITAVAS: C1 - C7):", "CLAVIER D'ÉCOUTE EN DIRECT (6 OCTAVES : C1 - C7) :");
    add("status.connected", "CONNECTED", "CONECTADO", "CONNESSO", "VERBUNDEN", "接続済", "已连接", "CONECTADO", "CONNECTÉ");
    add("status.disconnected", "DISCONNECTED", "DESCONECTADO", "DISCONNESSO", "GETRENNT", "未接続", "未连接", "DESCONECTADO", "DÉCONNECTÉ");
    add("status.bar", "MIDI In: %s | Out: %s | Ch: %02d | Mode: %s %c-%d (PC %d) | SysEx RX: %d (%zu B)",
        "MIDI In: %s | Out: %s | Canal: %02d | Modo: %s %c-%d (PC %d) | SysEx RX: %d (%zu B)",
        "MIDI In: %s | Out: %s | Can: %02d | Modo: %s %c-%d (PC %d) | SysEx RX: %d (%zu B)",
        "MIDI In: %s | Out: %s | Kan: %02d | Modus: %s %c-%d (PC %d) | SysEx RX: %d (%zu B)",
        "MIDI In: %s | Out: %s | Ch: %02d | モード: %s %c-%d (PC %d) | SysEx RX: %d (%zu B)",
        "MIDI In: %s | Out: %s | 通道: %02d | 模式: %s %c-%d (PC %d) | SysEx RX: %d (%zu B)",
        "MIDI In: %s | Out: %s | Canal: %02d | Modo: %s %c-%d (PC %d) | SysEx RX: %d (%zu B)",
        "MIDI In : %s | Out : %s | Can : %02d | Mode : %s %c-%d (PC %d) | SysEx RX : %d (%zu B)");

    // Librarian
    add("lib.title", "CZ-3000 BANK LIBRARIAN (64 VOICES)", "GESTOR DE BANCO CZ-3000 (64 TIMBRES)", "GESTORE BANCHI CZ-3000 (64 VOCI)", "CZ-3000 BANK-MANAGER (64 KLÄNGE)", "CZ-3000 バンクライブラリアン (64音色)", "CZ-3000 音色库管理器 (64音色)", "GERENCIADOR DE BANCO CZ-3000 (64 TIMBRES)", "GESTIONNAIRE DE BANQUE CZ-3000 (64 SONS)");
    add("lib.status", "Status: %s", "Estado: %s", "Stato: %s", "Status: %s", "状態: %s", "状态: %s", "Status: %s", "État : %s");
    add("lib.preset_tab", "PRESET BANK (32 Fixed)", "PRESET BANK (32 Fijos)", "PRESET BANK (32 Fissi)", "PRESET BANK (32 Fest)", "PRESET BANK (32固定)", "PRESET BANK (32 预设)", "PRESET BANK (32 Fixos)", "PRESET BANK (32 Fixes)");
    add("lib.memory_tab", "MEMORY BANK (32 User)", "MEMORY BANK (32 Usuario)", "MEMORY BANK (32 Utente)", "MEMORY BANK (32 User)", "MEMORY BANK (32ユーザー)", "MEMORY BANK (32 用户)", "MEMORY BANK (32 Usuário)", "MEMORY BANK (32 Util.)");
    add("lib.auto_fetch", "Auto-Fetch SysEx", "Auto-Leer SysEx", "Auto-Leggi SysEx", "Auto-SysEx-Abruf", "SysEx自動読込", "自动读取 SysEx", "Auto-Ler SysEx", "Lecture auto SysEx");
    add("lib.auto_fetch_tip", "When MIDI In and Out are connected, clicking a Preset or Memory requests its SysEx data to update envelopes immediately.",
        "Si MIDI In y Out están conectados, al hacer clic en un Preset o Memory solicita sus datos por SysEx para mostrar sus envolventes al instante.",
        "Se MIDI In e Out sono connessi, cliccando su Preset o Memory richiede i dati SysEx per aggiornare subito gli inviluppi.",
        "Wenn MIDI In & Out verbunden sind, werden beim Klick automatisch die SysEx-Daten für die Hüllkurven abgerufen.",
        "MIDI In/Out接続時、音色クリックでSysExを自動要求しエンベロープを即座に更新します。",
        "连接 MIDI 输入和输出时，点击音色会自动请求 SysEx 数据以立即更新包络线。",
        "Com MIDI In e Out conectados, clicar em um Preset ou Memory solicita o SysEx para atualizar os envelopes na hora.",
        "Lorsque MIDI In et Out sont connectés, un clic demande les données SysEx pour mettre à jour les enveloppes.");
    add("lib.copy_to_mem", "Copy to Memory Bank", "Copiar a Memory Bank", "Copia in Memory Bank", "In Memory Bank kopieren", "Memoryバンクへコピー", "复制到用户库", "Copiar para Memory Bank", "Copier vers Memory Bank");
    add("lib.read_preset", "Read Preset", "Leer Preset", "Leggi Preset", "Preset lesen", "プリセット読込", "读取预设", "Ler Preset", "Lire Preset");
    add("lib.cancel_dl", "Cancel Download", "Cancelar Descarga", "Annulla Download", "Download abbrechen", "読込中止", "取消下载", "Cancelar Download", "Annuler");
    add("lib.dl_32_presets", "Download 32 Presets", "Descargar 32 Presets", "Scarica 32 Preset", "32 Presets laden", "32プリセット一括読込", "下载 32 个预设", "Baixar 32 Presets", "Télécharger 32 Presets");
    add("lib.cache_ok", "[Cache OK]", "[Caché OK]", "[Cache OK]", "[Cache OK]", "[キャッシュ有]", "[已缓存]", "[Cache OK]", "[Cache OK]");
    add("lib.no_cache", "[No cache]", "[Sin caché]", "[No cache]", "[Kein Cache]", "[キャッシュ無]", "[未缓存]", "[Sem cache]", "[Sans cache]");
    add("lib.send_buf", "Send to Buffer (0x60)", "Enviar a Búfer (0x60)", "Invia a Buffer (0x60)", "An Puffer (0x60)", "バッファ送信 (0x60)", "发送到缓冲区 (0x60)", "Enviar ao Buffer (0x60)", "Envoyer au tampon (0x60)");
    add("lib.save_synth", "Write to Synth", "Guardar en Synth", "Scrivi su Synth", "In Synth speichern", "本体に書込", "写入合成器", "Gravar no Synth", "Écrire sur le Synth");
    add("lib.read_synth", "Read from Synth", "Leer del Synth", "Leggi da Synth", "Vom Synth lesen", "本体から読込", "从合成器读取", "Ler do Synth", "Lire du Synth");
    add("lib.dl_32_mem", "Download 32 Memories", "Descargar 32 Memorias", "Scarica 32 Memorie", "32 Speicher laden", "32メモリ一括読込", "下载 32 个用户音色", "Baixar 32 Memórias", "Télécharger 32 Mémoires");
    add("lib.send_bank", "Send Bank (32)", "Enviar Banco (32)", "Invia Banco (32)", "Bank senden (32)", "バンク送信 (32)", "发送音色库 (32)", "Enviar Banco (32)", "Envoyer banque (32)");
    add("lib.copy", "Copy", "Copiar", "Copia", "Kopieren", "コピー", "复制", "Copiar", "Copier");
    add("lib.paste", "Paste", "Pegar", "Incolla", "Einfügen", "貼付", "粘贴", "Colar", "Coller");
    add("lib.randomize", "Randomize", "Aleatorio", "Casuale", "Zufall", "ランダム", "随机生成", "Aleatório", "Aléatoire");
    add("lib.randomize_tip",
        "Randomize all synth parameters (DCO, DCW, DCA, envelopes, waveforms, vibrato, detune, modulation) in the current Memory slot.",
        "Aleatoriza todos los parámetros del sintetizador (DCO, DCW, DCA, envolventes, ondas, vibrato, detune, modulación) en la memoria actual.",
        "Genera valori casuali per tutti i parametri del synth (DCO, DCW, DCA, inviluppi, onde, vibrato, detune, modulazione) nello slot Memory attuale.",
        "Alle Synthesizer-Parameter (DCO, DCW, DCA, Hüllkurven, Wellenformen, Vibrato, Detune, Modulation) im aktuellen Memory-Platz zufällig erzeugen.",
        "現在のメモリー枠で全パラメーター（DCO/DCW/DCA/エンベロープ/波形/ビブラート/デチューン/変調）をランダム生成します。",
        "在当前记忆槽位随机生成所有合成器参数（DCO、DCW、DCA、包络、波形、颤音、失谐、调制）。",
        "Randomiza todos os parâmetros do sintetizador (DCO, DCW, DCA, envelopes, ondas, vibrato, detune, modulação) na memória atual.",
        "Génère aléatoirement tous les paramètres du synthé (DCO, DCW, DCA, enveloppes, ondes, vibrato, detune, modulation) dans la mémoire actuelle.");
    add("lib.sync_60", "Sync 0x60", "Sync 0x60", "Sync 0x60", "Sync 0x60", "0x60同期", "同步 0x60", "Sync 0x60", "Sync 0x60");
    add("lib.sync_60_tip", "When enabled, selecting a Memory patch also sends it to the CZ-3000 edit buffer (0x60) (useful after loading a .syx file).",
        "Al activar, al seleccionar una memoria de usuario se envía también al búfer 0x60 del CZ-3000 (útil tras cargar un .syx).",
        "Se attivo, selezionando una memoria la invia anche al buffer 0x60 del CZ-3000 (utile dopo aver caricato un .syx).",
        "Wenn aktiv, wird beim Auswählen eines User-Patches auch der Edit-Puffer 0x60 gesendet.",
        "有効時、メモリ選択時に編集バッファ(0x60)にも自動送信します（.syx読込後に便利）。",
        "启用后，选择用户音色时也会自动发送到 CZ-3000 编辑缓冲区 (0x60)。",
        "Quando ativo, selecionar uma memória também envia para o buffer 0x60 do CZ-3000.",
        "Si activé, la sélection d'une mémoire l'envoie aussi au tampon 0x60 du CZ-3000.");
    add("lib.bank_btn", "Bank %c##selbank%d", "Banco %c##selbank%d", "Banco %c##selbank%d", "Bank %c##selbank%d", "バンク %c##selbank%d", "音色组 %c##selbank%d", "Banco %c##selbank%d", "Banque %c##selbank%d");
    add("lib.rename", "Rename Memory %c-%d:", "Renombrar Memoria %c-%d:", "Rinomina Memoria %c-%d:", "Speicher %c-%d umbenennen:", "メモリ %c-%d の名前変更:", "重命名音色 %c-%d:", "Renomear Memória %c-%d:", "Renommer Mémoire %c-%d :");
    add("lib.done", "Done", "Listo", "Fatto", "Fertig", "完了", "完成", "Pronto", "Terminé");
    add("lib.ready", "Librarian ready.", "Librarian listo.", "Librarian pronto.", "Manager bereit.", "ライブラリアン準備完了", "音色库管理器已就绪", "Librarian pronto.", "Gestionnaire prêt.");

    // Envelope Graph
    add("env.title", "ENVELOPE GRAPH (8 STEPS):", "GRAFICO DE ENVOLVENTES (8 PASOS):", "GRAFICO INVILUPPI (8 PASSI):", "HÜLLKURVENGRAFIK (8 SCHRITTE):", "エンベロープグラフ (8ステップ):", "包络线图表 (8 步):", "GRÁFICO DE ENVELOPES (8 PASSOS):", "GRAPHIQUE D'ENVELOPPES (8 ÉTAPES) :");
    add("env.dco", "DCO (Pitch)", "DCO (Tono)", "DCO (Intonazione)", "DCO (Tonhöhe)", "DCO (ピッチ)", "DCO (音高)", "DCO (Afinação)", "DCO (Hauteur)");
    add("env.dcw", "DCW (Brightness / PD)", "DCW (Brillo/Filtro PD)", "DCW (Brillantezza / PD)", "DCW (Klangfarbe / PD)", "DCW (音色/PD)", "DCW (亮度/PD滤波)", "DCW (Brilho / PD)", "DCW (Brillance / PD)");
    add("env.dca", "DCA (Volume)", "DCA (Volumen)", "DCA (Volume)", "DCA (Lautstärke)", "DCA (音量)", "DCA (音量)", "DCA (Volume)", "DCA (Volume)");
    add("env.all", "ALL (Layered)", "TODAS (Superpuestas)", "TUTTI (Sovrapposti)", "ALLE (Überlagert)", "すべて (重ね表示)", "全部 (叠加显示)", "TODAS (Sobrepostas)", "TOUTES (Superposées)");
    add("env.editing", "EDITING: %s", "EDITANDO: %s", "MODIFICA: %s", "BEARBEITUNG: %s", "編集中: %s", "正在编辑: %s", "EDITANDO: %s", "ÉDITION : %s");
    add("env.step", "  Step:", "  Paso:", "  Passo:", "  Schritt:", "  ステップ:", "  步:", "  Passo:", "  Étape :");
    add("env.rate", "Rate (Speed)", "Rate (Velocidad)", "Rate (Velocità)", "Rate (Tempo)", "Rate (速度)", "Rate (速率)", "Rate (Velocidade)", "Rate (Vitesse)");
    add("env.level", "Level", "Level (Nivel)", "Level (Livello)", "Level (Pegel)", "Level (レベル)", "Level (电平)", "Level (Nível)", "Level (Niveau)");
    add("env.sustain", "Sustain (SUS)", "Sustain (SUS)", "Sustain (SUS)", "Sustain (SUS)", "サスティン (SUS)", "延音 (SUS)", "Sustain (SUS)", "Sustain (SUS)");
    add("env.end", "End (END)", "End (FIN)", "End (FINE)", "End (ENDE)", "エンド (END)", "结束 (END)", "End (FIM)", "End (FIN)");

    // Tutorial Window
    add("tut.badge", "TUTORIAL MODE", "MODO TUTORIAL", "MODALITÀ TUTORIAL", "TUTORIAL-MODUS", "チュートリアル", "教程模式", "MODO TUTORIAL", "MODE TUTORIEL");
    add("tut.win_title", "Casio CZ-3000 Interactive Phase Distortion Guide", "Guía Interactiva de Distorsión de Fase Casio CZ-3000", "Guida Interattiva Distorsione di Fase Casio CZ-3000", "Interaktives Casio CZ-3000 Phase-Distortion-Handbuch", "Casio CZ-3000 フェーズディストーション解説ガイド", "Casio CZ-3000 相位失真交互指南", "Guia Interativo de Distorção de Fase Casio CZ-3000", "Guide interactif de distorsion de phase Casio CZ-3000");
    add("tut.topics", "Synthesis Topics", "Temas de Síntesis", "Argomenti di Sintesi", "Synthese-Themen", "シンセシス項目", "合成主题", "Tópicos de Síntese", "Thèmes de synthèse");
    add("tut.effect", "Sonic Effect:", "Efecto Sonoro:", "Effetto Sonoro:", "Klangeffekt:", "音響効果:", "声音效果:", "Efeito Sonoro:", "Effet sonore :");
    add("tut.tech", "How it works in the CZ-3000:", "¿Cómo funciona en el CZ-3000?", "Come funziona nel CZ-3000:", "Funktionsweise im CZ-3000:", "CZ-3000での仕組み:", "CZ-3000 中的工作原理:", "Como funciona no CZ-3000:", "Fonctionnement dans le CZ-3000 :");

    // Official Manual Window
    add("manual.win_title", "Casio CZ-3000 Official Operation Manual", "Manual Oficial de Operación Casio CZ-3000", "Manuale Ufficiale d'Uso Casio CZ-3000", "Offizielle Casio CZ-3000 Bedienungsanleitung", "Casio CZ-3000 公式取扱説明書", "Casio CZ-3000 官方操作手册", "Manual Oficial de Operação Casio CZ-3000", "Manuel d'utilisation officiel Casio CZ-3000");
    add("manual.sections", "Manual Chapters", "Capítulos del Manual", "Capitoli del Manuale", "Handbuch-Kapitel", "マニュアル目次", "手册章节", "Capítulos do Manual", "Chapitres du manuel");
}

} // namespace cz
