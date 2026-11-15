#include "cz_vst_view.h"
#include "cz_vst_processor.h"
#include "../ui/cz_lcd.h"
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <cstdio>
#include <algorithm>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern HINSTANCE g_hModule;

namespace cz {
    extern void LogDebug(const std::string& msg);
}

namespace cz {

namespace {

bool VstCasioButton(const char* label, bool active, bool led = false,
                    ImVec2 size = ImVec2(48, 34),
                    ImU32 led_color = IM_COL32(255, 40, 40, 255)) {
    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();

    bool pressed = ImGui::InvisibleButton(label, size);
    bool hovered = ImGui::IsItemHovered();
    bool held = ImGui::IsItemActive();

    ImU32 btn_col = active ? IM_COL32(55, 60, 68, 255)
                           : (hovered ? IM_COL32(45, 50, 56, 255)
                                      : IM_COL32(36, 40, 45, 255));
    if (held) btn_col = IM_COL32(25, 28, 32, 255);

    ImVec2 p_min = pos;
    ImVec2 p_max = ImVec2(pos.x + size.x, pos.y + size.y);
    draw->AddRectFilled(p_min, p_max, btn_col, 3.0f);
    draw->AddRect(p_min, p_max, held ? IM_COL32(20, 20, 20, 255) : IM_COL32(70, 78, 88, 255), 3.0f, 0, 1.0f);

    draw->AddLine(ImVec2(p_min.x + 2, p_min.y + 1), ImVec2(p_max.x - 2, p_min.y + 1),
                  IM_COL32(255, 255, 255, held ? 10 : 40));

    if (led || active) {
        ImVec2 led_pos = ImVec2(pos.x + (size.x * 0.5f), pos.y + 5.0f);
        ImU32 col = active ? led_color : IM_COL32(60, 20, 20, 255);
        draw->AddCircleFilled(led_pos, 2.5f, col);
        if (active) {
            draw->AddCircleFilled(led_pos, 4.5f, (led_color & 0x00FFFFFF) | 0x50000000);
        }
    }

    const char* text_end = strstr(label, "##");
    if (!text_end) text_end = label + strlen(label);
    ImVec2 text_size = ImGui::CalcTextSize(label, text_end);
    float text_y = pos.y + (size.y * 0.5f) - (text_size.y * 0.5f) + (led || active ? 2.5f : 0.0f);
    draw->AddText(ImVec2(pos.x + (size.x - text_size.x) * 0.5f, text_y),
                  IM_COL32(230, 235, 240, 255), label, text_end);

    return pressed;
}

} // namespace

CzVstView::CzVstView(CzVstProcessor& processor)
    : processor_(processor) {
}

CzVstView::~CzVstView() {
    removed();
}

Steinberg::tresult PLUGIN_API CzVstView::queryInterface(const Steinberg::TUID _iid, void** obj) {
    if (!_iid || !obj) return Steinberg::kInvalidArgument;
    if (memcmp(_iid, Steinberg::FUnknown::iid, 16) == 0 ||
        memcmp(_iid, Steinberg::Vst::IPlugView::iid, 16) == 0) {
        *obj = static_cast<Steinberg::Vst::IPlugView*>(this);
        addRef();
        return Steinberg::kResultOk;
    }
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 PLUGIN_API CzVstView::addRef() {
    return ++ref_count_;
}

Steinberg::uint32 PLUGIN_API CzVstView::release() {
    uint32_t r = --ref_count_;
    if (r == 0) {
        delete this;
    }
    return r;
}

Steinberg::tresult PLUGIN_API CzVstView::isPlatformTypeSupported(const Steinberg::char8* type) {
    if (type && strcmp(type, "HWND") == 0) {
        return Steinberg::kResultTrue;
    }
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API CzVstView::getSize(Steinberg::Vst::ViewRect* size) {
    if (!size) return Steinberg::kInvalidArgument;
    size->left = 0;
    size->top = 0;
    size->right = kViewWidth;
    size->bottom = kViewHeight;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::onSize(Steinberg::Vst::ViewRect* /*newSize*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::onFocus(Steinberg::TBool /*state*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::setFrame(void* /*frame*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::canResize() {
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API CzVstView::checkSizeConstraint(Steinberg::Vst::ViewRect* /*rect*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::onWheel(float /*distance*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::onKeyDown(Steinberg::char16 /*key*/, Steinberg::int16 /*keyCode*/, Steinberg::int16 /*modifiers*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::onKeyUp(Steinberg::char16 /*key*/, Steinberg::int16 /*keyCode*/, Steinberg::int16 /*modifiers*/) {
    return Steinberg::kResultOk;
}

LRESULT CALLBACK CzVstView::ViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    CzVstView* view = reinterpret_cast<CzVstView*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (view && view->imgui_initialized_ && view->imgui_context_) {
        ImGui::SetCurrentContext(view->imgui_context_);
        if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
            return true;
        }
    }

    switch (msg) {
    case WM_PAINT:
        if (view) {
            view->render_frame();
        }
        ValidateRect(hwnd, nullptr);
        return 0;
    case WM_TIMER:
        if (view) {
            view->render_frame();
        }
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_DESTROY:
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool CzVstView::init_d3d(HWND hwnd) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = kViewWidth;
    sd.BufferDesc.Height = kViewHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
                                               featureLevels, 2, D3D11_SDK_VERSION, &sd,
                                               &swap_chain_, &d3d_device_, &featureLevel, &d3d_context_);
    if (res != S_OK) {
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
                                           featureLevels, 2, D3D11_SDK_VERSION, &sd,
                                           &swap_chain_, &d3d_device_, &featureLevel, &d3d_context_);
    }
    if (res != S_OK) return false;

    create_render_target();
    return true;
}

void CzVstView::create_render_target() {
    ID3D11Texture2D* pBackBuffer = nullptr;
    if (swap_chain_) {
        swap_chain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer) {
            d3d_device_->CreateRenderTargetView(pBackBuffer, nullptr, &render_target_);
            pBackBuffer->Release();
        }
    }
}

void CzVstView::cleanup_render_target() {
    if (render_target_) {
        render_target_->Release();
        render_target_ = nullptr;
    }
}

void CzVstView::cleanup_d3d() {
    cleanup_render_target();
    if (swap_chain_) { swap_chain_->Release(); swap_chain_ = nullptr; }
    if (d3d_context_) { d3d_context_->Release(); d3d_context_ = nullptr; }
    if (d3d_device_) { d3d_device_->Release(); d3d_device_ = nullptr; }
}

Steinberg::tresult PLUGIN_API CzVstView::attached(void* parent, const Steinberg::char8* type) {
    LogDebug(std::string("CzVstView::attached called with type: ") + (type ? type : "null"));
    if (!parent) {
        return Steinberg::kInvalidArgument;
    }
    if (type && strcmp(type, "HWND") != 0) {
        return Steinberg::kInvalidArgument;
    }
    parent_hwnd_ = static_cast<HWND>(parent);

    HINSTANCE hInst = g_hModule ? g_hModule : GetModuleHandle(nullptr);
    WNDCLASSEXA wc{};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = ViewWndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "CZ3000SysExVst3View";
    RegisterClassExA(&wc);

    hwnd_ = CreateWindowExA(0, wc.lpszClassName, "CZ-3000 VST3 View",
                            WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                            0, 0, kViewWidth, kViewHeight,
                            parent_hwnd_, nullptr, hInst, nullptr);
    if (!hwnd_) {
        LogDebug("CzVstView::attached CreateWindowExA failed");
        return Steinberg::kInternalError;
    }

    SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    if (!init_d3d(hwnd_)) {
        LogDebug("CzVstView::attached init_d3d failed");
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
        return Steinberg::kInternalError;
    }

    IMGUI_CHECKVERSION();
    imgui_context_ = ImGui::CreateContext();
    ImGui::SetCurrentContext(imgui_context_);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = nullptr; // Do not write imgui.ini inside DAW process directory

    // Styling
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.Colors[ImGuiCol_WindowBg]         = ImVec4(0.12f, 0.14f, 0.16f, 1.0f);
    style.Colors[ImGuiCol_Header]           = ImVec4(0.18f, 0.40f, 0.45f, 0.8f);
    style.Colors[ImGuiCol_HeaderHovered]    = ImVec4(0.20f, 0.60f, 0.65f, 0.8f);
    style.Colors[ImGuiCol_HeaderActive]     = ImVec4(0.25f, 0.70f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_Button]           = ImVec4(0.18f, 0.22f, 0.26f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered]    = ImVec4(0.25f, 0.32f, 0.38f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive]     = ImVec4(0.12f, 0.50f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_FrameBg]          = ImVec4(0.16f, 0.18f, 0.22f, 1.0f);

    io.Fonts->AddFontDefault();

    ImGui_ImplWin32_Init(hwnd_);
    ImGui_ImplDX11_Init(d3d_device_, d3d_context_);
    imgui_initialized_ = true;

    // 30 FPS update timer for UI responsiveness
    SetTimer(hwnd_, 1001, 33, nullptr);

    LogDebug("CzVstView::attached succeeded");
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API CzVstView::removed() {
    LogDebug("CzVstView::removed called");
    if (hwnd_) {
        KillTimer(hwnd_, 1001);
        if (imgui_initialized_) {
            if (imgui_context_) {
                ImGui::SetCurrentContext(imgui_context_);
                ImGui_ImplDX11_Shutdown();
                ImGui_ImplWin32_Shutdown();
                ImGui::DestroyContext(imgui_context_);
                imgui_context_ = nullptr;
            }
            imgui_initialized_ = false;
        }
        cleanup_d3d();
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
    return Steinberg::kResultOk;
}

void CzVstView::render_frame() {
    if (!imgui_initialized_ || !imgui_context_ || !d3d_context_ || !swap_chain_ || !render_target_) return;

    ImGui::SetCurrentContext(imgui_context_);
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(kViewWidth), static_cast<float>(kViewHeight)));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;

    ImGui::Begin("CZ3000VstMain", nullptr, flags);

    ImDrawList* draw = ImGui::GetWindowDrawList();
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    float w = static_cast<float>(kViewWidth);
    float h = static_cast<float>(kViewHeight);

    // Chassis
    draw->AddRectFilled(p0, ImVec2(p0.x + w, p0.y + h), IM_COL32(28, 31, 35, 255), 4.0f);
    draw->AddRect(p0, ImVec2(p0.x + w, p0.y + h), IM_COL32(50, 56, 64, 255), 4.0f, 0, 1.5f);

    // Decorative Casio colored stripes
    float sy = p0.y + 36.0f;
    draw->AddLine(ImVec2(p0.x + 8, sy), ImVec2(p0.x + w - 8, sy), IM_COL32(0, 175, 195, 255), 2.0f);
    draw->AddLine(ImVec2(p0.x + 8, sy + 3), ImVec2(p0.x + w - 8, sy + 3), IM_COL32(30, 90, 180, 255), 1.5f);
    draw->AddLine(ImVec2(p0.x + 8, sy + 6), ImVec2(p0.x + w - 8, sy + 6), IM_COL32(220, 45, 45, 255), 1.5f);

    // Top Header: Branding
    ImGui::SetCursorPos(ImVec2(14, 10));
    ImGui::TextColored(ImVec4(0.95f, 0.95f, 0.95f, 1.0f), "CASIO");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.92f, 0.84f, 0.60f, 1.0f), "CZ-3000");
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.6f, 0.65f, 0.7f, 1.0f), "VST3 MIDI LIBRARIAN");

    // MIDI Port & Channel Bar on top right
    ImGui::SameLine(w - 440);
    ImGui::SetNextItemWidth(170);
    auto& midi = processor_.get_midi();
    auto out_devs = midi.get_output_devices();
    std::string cur_name = processor_.get_midi_output_name();
    if (ImGui::BeginCombo("##MidiPort", cur_name.empty() ? "(No MIDI Out)" : cur_name.c_str())) {
        for (const auto& dev : out_devs) {
            bool sel = (dev.name == cur_name);
            if (ImGui::Selectable(dev.name.c_str(), sel)) {
                processor_.connect_midi_output_by_name(dev.name);
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Direct Hardware MIDI Out Port");

    ImGui::SameLine();
    ImGui::SetNextItemWidth(55);
    int cur_ch = processor_.get_midi_channel();
    if (ImGui::BeginCombo("##MidiCh", ("Ch " + std::to_string(cur_ch)).c_str())) {
        for (int c = 1; c <= 16; ++c) {
            char ch_label[16];
            snprintf(ch_label, sizeof(ch_label), "Ch %d", c);
            if (ImGui::Selectable(ch_label, c == cur_ch)) {
                processor_.set_midi_channel(c);
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("MIDI Channel (1-16)");

    ImGui::SameLine();
    if (ImGui::SmallButton("Sync Banks")) {
        processor_.reload_banks_from_cache();
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reload 64 patches from standalone app cache");

    // Main Body
    int cur_prog = processor_.get_current_program();
    BankMode active_mode = (cur_prog < 32) ? BankMode::Preset : BankMode::Memory;
    size_t active_idx = static_cast<size_t>(cur_prog % 32);
    auto [active_b, active_s] = CzBank::get_bank_slot(active_idx);

    CzBank& bank = processor_.get_bank();
    const auto& active_patch = bank.get_patch_by_index(active_mode, active_idx);

    // Left Column: LCD + Casio Tactile Buttons
    ImGui::SetCursorPos(ImVec2(14, 52));
    ImGui::BeginGroup();

    // 16x2 HD44780 LCD
    char l1[32], l2[32];
    snprintf(l1, sizeof(l1), "%-8s %c-%d  PC%02d",
             (active_mode == BankMode::Preset ? "*PRESET*" : "*MEMORY*"),
             active_b, active_s, cur_prog);
    snprintf(l2, sizeof(l2), "%-16.16s", active_patch.name.c_str());

    CzLcd lcd;
    lcd.set_lines(l1, l2);
    lcd.render(1.0f);

    ImGui::Spacing();
    ImGui::Spacing();

    // Tactile Hardware Deck
    static const char bank_chars[] = {'A', 'B', 'C', 'D'};
    const float btn_w = 42.0f;
    const float btn_h = 32.0f;
    const float gap = 4.0f;

    // Row 1: PRESET (A-D) and MEMORY (A-D)
    ImGui::TextColored(ImVec4(0.35f, 0.9f, 0.65f, 1.0f), "PRESET:");
    ImGui::SameLine(62);
    for (int b = 0; b < 4; ++b) {
        if (b > 0) ImGui::SameLine(0, gap);
        char lbl[16];
        snprintf(lbl, sizeof(lbl), "%c##vst_pre", bank_chars[b]);
        bool act = (active_mode == BankMode::Preset && active_b == bank_chars[b]);
        if (VstCasioButton(lbl, act, true, ImVec2(btn_w, btn_h), IM_COL32(40, 230, 140, 255))) {
            size_t new_idx = CzBank::get_index(bank_chars[b], active_s);
            processor_.trigger_program_change(static_cast<int>(new_idx));
        }
    }

    ImGui::SameLine(0, 14);
    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "MEMORY:");
    ImGui::SameLine(295);
    for (int b = 0; b < 4; ++b) {
        if (b > 0) ImGui::SameLine(0, gap);
        char lbl[16];
        snprintf(lbl, sizeof(lbl), "%c##vst_mem", bank_chars[b]);
        bool act = (active_mode == BankMode::Memory && active_b == bank_chars[b]);
        if (VstCasioButton(lbl, act, true, ImVec2(btn_w, btn_h), IM_COL32(255, 160, 40, 255))) {
            size_t new_idx = CzBank::get_index(bank_chars[b], active_s);
            processor_.trigger_program_change(static_cast<int>(32 + new_idx));
        }
    }

    ImGui::Spacing();

    // Row 2: VOICE SELECTORS (1..8)
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "VOICE:");
    ImGui::SameLine(62);
    for (int s = 1; s <= 8; ++s) {
        if (s > 1) ImGui::SameLine(0, gap + (s == 5 ? 14.0f : 0.0f));
        char lbl[16];
        snprintf(lbl, sizeof(lbl), "%d##vst_v", s);
        bool act = (active_s == s);
        if (VstCasioButton(lbl, act, true, ImVec2(btn_w, btn_h), IM_COL32(255, 60, 60, 255))) {
            size_t new_idx = CzBank::get_index(active_b, s);
            int base = (active_mode == BankMode::Preset ? 0 : 32);
            processor_.trigger_program_change(base + static_cast<int>(new_idx));
        }
    }

    ImGui::Spacing();
    ImGui::Spacing();

    // Action Buttons
    if (ImGui::Button("SEND PROGRAM CHANGE", ImVec2(180, 28))) {
        processor_.trigger_program_change(cur_prog);
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Send MIDI Program Change (%d) to synth", cur_prog);

    ImGui::SameLine(0, 10);
    if (ImGui::Button("SEND SYSEX 0x60 (RECALL)", ImVec2(180, 28))) {
        processor_.send_patch_to_recall(cur_prog);
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Send current patch parameters to COMPARE/RECALL buffer via SysEx");

    ImGui::Spacing();
    bool auto_buf = processor_.get_auto_send_memory_buffer();
    if (ImGui::Checkbox("AUTO-SEND MEMORY TO BUFFER (0x60)", &auto_buf)) {
        processor_.set_auto_send_memory_buffer(auto_buf);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("When ON (default), selecting any Memory patch (A-1..D-8) automatically sends its parameters to the synth's COMPARE/RECALL buffer (0x60) via SysEx so it makes sound immediately");
    }

    ImGui::EndGroup();

    // Right Column: Librarian 32-Preset / 32-Memory List View
    ImGui::SetCursorPos(ImVec2(480, 52));
    ImGui::BeginGroup();

    ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.85f, 1.0f), "64 PATCH LIBRARIAN");

    // Mode Selector
    bool is_pre = (active_mode == BankMode::Preset);
    if (is_pre) {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(30, 140, 95, 255));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(42, 46, 52, 255));
    }
    if (ImGui::Button("PRESETS (32)", ImVec2(148, 24))) {
        processor_.trigger_program_change(static_cast<int>(active_idx));
    }
    ImGui::PopStyleColor();

    ImGui::SameLine();
    if (!is_pre) {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(190, 95, 30, 255));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(42, 46, 52, 255));
    }
    if (ImGui::Button("MEMORIES (32)", ImVec2(148, 24))) {
        processor_.trigger_program_change(static_cast<int>(32 + active_idx));
    }
    ImGui::PopStyleColor();

    // Bank A, B, C, D Tabs
    for (int b = 0; b < 4; ++b) {
        if (b > 0) ImGui::SameLine();
        bool is_act_bank = (viewed_bank_ == b);
        if (is_act_bank) {
            ImGui::PushStyleColor(ImGuiCol_Button, is_pre ? IM_COL32(30, 140, 95, 255) : IM_COL32(0, 145, 165, 255));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(42, 46, 52, 255));
        }
        char b_lbl[16];
        snprintf(b_lbl, sizeof(b_lbl), "Bank %c", bank_chars[b]);
        if (ImGui::Button(b_lbl, ImVec2(72, 22))) {
            viewed_bank_ = b;
        }
        ImGui::PopStyleColor();
    }

    // 8 Patch Slots for Selected Bank
    char v_bank_char = bank_chars[viewed_bank_];
    ImGui::BeginChild("VstSoundList", ImVec2(310, 245), true);
    for (int s = 1; s <= 8; ++s) {
        size_t idx = CzBank::get_index(v_bank_char, s);
        int prog_num = (is_pre ? 0 : 32) + static_cast<int>(idx);
        const auto& p = bank.get_patch_by_index(is_pre ? BankMode::Preset : BankMode::Memory, idx);

        char slot_lbl[64];
        snprintf(slot_lbl, sizeof(slot_lbl), "%c-%d: %s", v_bank_char, s, p.name.c_str());

        bool sel = (cur_prog == prog_num);
        if (ImGui::Selectable(slot_lbl, sel)) {
            processor_.trigger_program_change(prog_num);
        }
    }
    ImGui::EndChild();

    ImGui::EndGroup();

    ImGui::End();

    // Render ImGui to Direct3D 11
    ImGui::Render();
    const float clear_color[4] = { 0.10f, 0.12f, 0.14f, 1.0f };
    d3d_context_->OMSetRenderTargets(1, &render_target_, nullptr);
    d3d_context_->ClearRenderTargetView(render_target_, clear_color);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    swap_chain_->Present(1, 0);
}

} // namespace cz
