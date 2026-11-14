#pragma once

#include <string>
#include <vector>

namespace cz {

struct ManualSection {
    std::string id;
    std::string title;
    std::string subtitle;
    std::string body;
};

class CzManual {
public:
    static CzManual& instance();
    void render_manual_window(bool* p_open);

private:
    CzManual();
    void init_sections();
    void render_preset_table();
    void render_initialize_table();
    void render_midi_chart();

    std::string active_section_id_ = "01_general";
    std::vector<ManualSection> sections_[8];
};

} // namespace cz
