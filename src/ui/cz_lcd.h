#pragma once

#include <string>
#include <array>
#include "imgui.h"

namespace cz {

class CzLcd {
public:
    CzLcd();

    void set_text(int row, const std::string& text);
    void set_lines(const std::string& line1, const std::string& line2);
    void clear();

    void set_cursor(int row, int col, bool visible = true);
    void set_backlight_color(ImU32 color);

    // Renders the authentic 16x2 LCD display into the current ImGui window
    void render(float scale = 1.0f);

    const std::string& get_line(int row) const { return (row == 0) ? line1_ : line2_; }

private:
    std::string line1_ = "1: BRASS ENS. 1 ";
    std::string line2_ = "OCTAVE: 0  M:INT";

    int cursor_row_ = 0;
    int cursor_col_ = -1; // -1 = off
    bool cursor_visible_ = true;
    double last_blink_time_ = 0.0;
    bool blink_state_ = true;

    ImU32 bg_color_ = IM_COL32(24, 40, 20, 255);       // Deep LCD olive background
    ImU32 char_on_color_ = IM_COL32(190, 245, 140, 255); // Glowing phosphor greenish-yellow
    ImU32 char_off_color_ = IM_COL32(32, 54, 28, 60);    // Inactive dot matrix shadow
    ImU32 border_color_ = IM_COL32(10, 15, 10, 255);    // LCD bezel
};

} // namespace cz

