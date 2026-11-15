#pragma once

#include "../core/cz_types.h"
#include "imgui.h"

namespace cz {

class CzEnvelopeGraph {
public:
    CzEnvelopeGraph();

    // Renders interactive 8-step envelope editor / curve plot
    // Returns true if any parameter was modified by mouse interaction
    bool render(CzPatch& patch, int active_line = 1, float width = 580.0f, float height = 180.0f);

    // Renders the Casio CZ-3000 faceplate silkscreen reference panel (Vibrato waves,
    // PD Envelope schematic, and Parameter Variable Range table) beside the curves.
    // Scales uniformly with `scale` so its factory aspect ratio never stretches.
    void render_casio_spec_plate(const CzPatch& patch, float scale = 1.0f);

    void set_active_section(int section) { active_section_ = section; } // 0: DCO, 1: DCW, 2: DCA, 3: ALL
    int get_active_section() const { return active_section_; }

    void set_selected_step(int step) { selected_step_ = step; } // 1 - 8
    int get_selected_step() const { return selected_step_; }

private:
    int active_section_ = 1; // Default: DCW (The Casio filter!)
    int selected_step_ = 1;  // Step 1 selected
    bool show_dco_ = true;
    bool show_dcw_ = true;
    bool show_dca_ = true;

    int dragging_point_ = -1; // -1 none
};

} // namespace cz

