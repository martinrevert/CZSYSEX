#include "cz_lcd.h"
#include <chrono>
#include <algorithm>

namespace cz {

// 5x7 dot matrix font bitmap definitions for common characters (ASCII 32 to 95)
// Each char is 5 columns of 7 bits (bit 0 = top, bit 6 = bottom)
namespace {

const uint8_t FONT_5X7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 32 ' '
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // 33 '!'
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 34 '"'
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // 35 '#'
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // 36 '$'
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 37 '%'
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 38 '&'
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 39 '''
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // 40 '('
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // 41 ')'
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // 42 '*'
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // 43 '+'
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 44 ','
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 45 '-'
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 46 '.'
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 47 '/'
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 48 '0'
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 49 '1'
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 50 '2'
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 51 '3'
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 52 '4'
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 53 '5'
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 54 '6'
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 55 '7'
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 56 '8'
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 57 '9'
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 58 ':'
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 59 ';'
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 60 '<'
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 61 '='
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 62 '>'
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 63 '?'
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // 64 '@'
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 65 'A'
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 66 'B'
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 67 'C'
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 68 'D'
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 69 'E'
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 70 'F'
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 71 'G'
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 72 'H'
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 73 'I'
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 74 'J'
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 75 'K'
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 76 'L'
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 77 'M'
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 78 'N'
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 79 'O'
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 80 'P'
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 81 'Q'
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 82 'R'
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 83 'S'
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 84 'T'
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 85 'U'
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 86 'V'
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // 87 'W'
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 88 'X'
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 89 'Y'
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 90 'Z'
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // 91 '['
    {0x02, 0x04, 0x08, 0x10, 0x20}, // 92 '\'
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // 93 ']'
    {0x04, 0x02, 0x01, 0x02, 0x04}, // 94 '^'
    {0x40, 0x40, 0x40, 0x40, 0x40}  // 95 '_'
};

const uint8_t* get_font_glyph(char c) {
    if (c >= 'a' && c <= 'z') c = static_cast<char>(c - 32); // To uppercase
    if (c >= 32 && c <= 95) {
        return FONT_5X7[c - 32];
    }
    return FONT_5X7[0]; // Space
}

} // anonymous namespace

CzLcd::CzLcd() {
    clear();
}

void CzLcd::clear() {
    line1_ = std::string(16, ' ');
    line2_ = std::string(16, ' ');
}

void CzLcd::set_text(int row, const std::string& text) {
    std::string s = text;
    if (s.size() < 16) s.resize(16, ' ');
    else if (s.size() > 16) s = s.substr(0, 16);

    if (row == 0) line1_ = s;
    else line2_ = s;
}

void CzLcd::set_lines(const std::string& line1, const std::string& line2) {
    set_text(0, line1);
    set_text(1, line2);
}

void CzLcd::set_cursor(int row, int col, bool visible) {
    cursor_row_ = row;
    cursor_col_ = col;
    cursor_visible_ = visible;
}

void CzLcd::set_backlight_color(ImU32 color) {
    bg_color_ = color;
}

void CzLcd::render(float scale) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    // Blink timer (500ms cycle)
    double now = ImGui::GetTime();
    if (now - last_blink_time_ > 0.4) {
        blink_state_ = !blink_state_;
        last_blink_time_ = now;
    }

    // Geometry
    const float dot_size = 2.2f * scale;
    const float dot_spacing = 0.8f * scale;
    const float char_w = (5 * dot_size) + (4 * dot_spacing);
    const float char_h = (7 * dot_size) + (6 * dot_spacing);
    const float char_spacing_x = 3.5f * scale;
    const float char_spacing_y = 6.0f * scale;

    const float pad_x = 12.0f * scale;
    const float pad_y = 10.0f * scale;

    const float lcd_w = (pad_x * 2.0f) + (16 * char_w) + (15 * char_spacing_x);
    const float lcd_h = (pad_y * 2.0f) + (2 * char_h) + char_spacing_y;

    // Draw Bezel / Recessed Housing
    ImVec2 outer_min = ImVec2(p.x - 4.0f * scale, p.y - 4.0f * scale);
    ImVec2 outer_max = ImVec2(p.x + lcd_w + 4.0f * scale, p.y + lcd_h + 4.0f * scale);
    draw_list->AddRectFilled(outer_min, outer_max, IM_COL32(15, 18, 15, 255), 4.0f * scale);
    draw_list->AddRect(outer_min, outer_max, IM_COL32(60, 65, 60, 255), 4.0f * scale, 0, 1.5f * scale);

    // Inner LCD Screen Glass
    ImVec2 screen_min = p;
    ImVec2 screen_max = ImVec2(p.x + lcd_w, p.y + lcd_h);
    draw_list->AddRectFilled(screen_min, screen_max, bg_color_, 2.0f * scale);

    // Subtle glass reflection gradient line
    draw_list->AddLine(
        ImVec2(screen_min.x + 2, screen_min.y + 2),
        ImVec2(screen_max.x - 2, screen_min.y + 2),
        IM_COL32(255, 255, 255, 30), 1.0f
    );

    // Render characters
    for (int row = 0; row < 2; ++row) {
        const std::string& line = (row == 0) ? line1_ : line2_;
        for (int col = 0; col < 16; ++col) {
            char c = (col < static_cast<int>(line.size())) ? line[col] : ' ';
            const uint8_t* glyph = get_font_glyph(c);

            float cx = p.x + pad_x + (col * (char_w + char_spacing_x));
            float cy = p.y + pad_y + (row * (char_h + char_spacing_y));

            bool is_cursor_pos = (cursor_visible_ && cursor_row_ == row && cursor_col_ == col && blink_state_);

            for (int x = 0; x < 5; ++x) {
                uint8_t col_bits = glyph[x];
                for (int y = 0; y < 7; ++y) {
                    bool dot_on = (col_bits & (1 << y)) != 0;

                    // Cursor underline on row 6
                    if (is_cursor_pos && y == 6) {
                        dot_on = true;
                    }

                    ImU32 dot_color = dot_on ? char_on_color_ : char_off_color_;

                    float dx = cx + (x * (dot_size + dot_spacing));
                    float dy = cy + (y * (dot_size + dot_spacing));

                    draw_list->AddRectFilled(
                        ImVec2(dx, dy),
                        ImVec2(dx + dot_size, dy + dot_size),
                        dot_color, 0.4f
                    );
                }
            }
        }
    }

    // Advance ImGui layout
    ImGui::Dummy(ImVec2(lcd_w, lcd_h));
}

} // namespace cz

