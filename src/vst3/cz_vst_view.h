#pragma once

#include "vst3_abi.h"
#include <atomic>
#include <windows.h>
#include <d3d11.h>

struct ImGuiContext;

namespace cz {

class CzVstProcessor;

class CzVstView : public Steinberg::Vst::IPlugView {
public:
    explicit CzVstView(CzVstProcessor& processor);
    virtual ~CzVstView();

    // FUnknown
    virtual Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void** obj) override;
    virtual Steinberg::uint32 PLUGIN_API addRef() override;
    virtual Steinberg::uint32 PLUGIN_API release() override;

    // IPlugView
    virtual Steinberg::tresult PLUGIN_API isPlatformTypeSupported(const Steinberg::char8* type) override;
    virtual Steinberg::tresult PLUGIN_API attached(void* parent, const Steinberg::char8* type) override;
    virtual Steinberg::tresult PLUGIN_API removed() override;
    virtual Steinberg::tresult PLUGIN_API onWheel(float distance) override;
    virtual Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) override;
    virtual Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode, Steinberg::int16 modifiers) override;
    virtual Steinberg::tresult PLUGIN_API getSize(Steinberg::Vst::ViewRect* size) override;
    virtual Steinberg::tresult PLUGIN_API onSize(Steinberg::Vst::ViewRect* newSize) override;
    virtual Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) override;
    virtual Steinberg::tresult PLUGIN_API setFrame(void* frame) override;
    virtual Steinberg::tresult PLUGIN_API canResize() override;
    virtual Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::Vst::ViewRect* rect) override;

    void render_frame();
    static LRESULT CALLBACK ViewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    bool init_d3d(HWND hwnd);
    void cleanup_d3d();
    void create_render_target();
    void cleanup_render_target();

    std::atomic<uint32_t> ref_count_{1};
    CzVstProcessor& processor_;

    HWND parent_hwnd_ = nullptr;
    HWND hwnd_ = nullptr;

    ID3D11Device* d3d_device_ = nullptr;
    ID3D11DeviceContext* d3d_context_ = nullptr;
    IDXGISwapChain* swap_chain_ = nullptr;
    ID3D11RenderTargetView* render_target_ = nullptr;

    bool imgui_initialized_ = false;
    ::ImGuiContext* imgui_context_ = nullptr;
    int viewed_bank_ = 0;

    static constexpr int kViewWidth = 820;
    static constexpr int kViewHeight = 440;
};

} // namespace cz
