#pragma once

#include "vst3_abi.h"
#include "../core/cz_bank.h"
#include "../midi/cz_midi.h"
#include <atomic>
#include <string>
#include <mutex>

namespace cz {

// 8A9B1C3D-4E5F-6A7B-8C9D-0E1F2A3B4C5D
static const Steinberg::TUID kCZ3000ProcessorUID = 
    SMTG_INLINE_UID(0x8A9B1C3D, 0x4E5F6A7B, 0x8C9D0E1F, 0x2A3B4C5D);

// 8A9B1C3D-4E5F-6A7B-8C9D-0E1F2A3B4C5E
static const Steinberg::TUID kCZ3000ControllerUID = 
    SMTG_INLINE_UID(0x8A9B1C3D, 0x4E5F6A7B, 0x8C9D0E1F, 0x2A3B4C5E);

enum ParamIDs : Steinberg::Vst::ParamID {
    kParamProgram = 0,
    kParamMidiChannel = 1,
    kParamCount = 2
};

class CzVstProcessor : public Steinberg::Vst::IComponent,
                       public Steinberg::Vst::IAudioProcessor,
                       public Steinberg::Vst::IEditController {
public:
    CzVstProcessor();
    virtual ~CzVstProcessor();

    // FUnknown
    virtual Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID _iid, void** obj) override;
    virtual Steinberg::uint32 PLUGIN_API addRef() override;
    virtual Steinberg::uint32 PLUGIN_API release() override;

    // IPluginBase
    virtual Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
    virtual Steinberg::tresult PLUGIN_API terminate() override;

    // IComponent
    virtual Steinberg::tresult PLUGIN_API getControllerClassId(Steinberg::TUID classId) override;
    virtual Steinberg::tresult PLUGIN_API setIoMode(Steinberg::Vst::IoMode mode) override;
    virtual Steinberg::int32 PLUGIN_API getBusCount(Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir) override;
    virtual Steinberg::tresult PLUGIN_API getBusInfo(Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir, Steinberg::int32 index, Steinberg::Vst::BusInfo& bus) override;
    virtual Steinberg::tresult PLUGIN_API getRoutingInfo(Steinberg::Vst::RoutingInfo& inInfo, Steinberg::Vst::RoutingInfo& outInfo) override;
    virtual Steinberg::tresult PLUGIN_API activateBus(Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir, Steinberg::int32 index, Steinberg::TBool state) override;
    virtual Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override;
    virtual Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override;
    virtual Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override;

    // IAudioProcessor
    virtual Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns, Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts) override;
    virtual Steinberg::tresult PLUGIN_API getBusArrangement(Steinberg::Vst::BusDirection dir, Steinberg::int32 index, Steinberg::Vst::SpeakerArrangement& arr) override;
    virtual Steinberg::tresult PLUGIN_API canProcessSampleSize(Steinberg::int32 symbolicSampleSize) override;
    virtual Steinberg::uint32 PLUGIN_API getLatencySamples() override;
    virtual Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup& setup) override;
    virtual Steinberg::tresult PLUGIN_API setProcessing(Steinberg::TBool state) override;
    virtual Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;
    virtual Steinberg::uint32 PLUGIN_API getTailSamples() override;

    // IEditController
    virtual Steinberg::tresult PLUGIN_API setComponentState(Steinberg::IBStream* state) override;
    virtual Steinberg::int32 PLUGIN_API getParameterCount() override;
    virtual Steinberg::tresult PLUGIN_API getParameterInfo(Steinberg::int32 paramIndex, Steinberg::Vst::ParameterInfo& info) override;
    virtual Steinberg::tresult PLUGIN_API getParamStringByValue(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized, Steinberg::char16* string) override;
    virtual Steinberg::tresult PLUGIN_API getParamValueByString(Steinberg::Vst::ParamID id, const Steinberg::char16* string, Steinberg::Vst::ParamValue& valueNormalized) override;
    virtual Steinberg::Vst::ParamValue PLUGIN_API normalizedParamToPlain(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue valueNormalized) override;
    virtual Steinberg::Vst::ParamValue PLUGIN_API plainParamToNormalized(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue plainValue) override;
    virtual Steinberg::Vst::ParamValue PLUGIN_API getParamNormalized(Steinberg::Vst::ParamID id) override;
    virtual Steinberg::tresult PLUGIN_API setParamNormalized(Steinberg::Vst::ParamID id, Steinberg::Vst::ParamValue value) override;
    virtual Steinberg::tresult PLUGIN_API setComponentHandler(Steinberg::Vst::IComponentHandler* handler) override;
    virtual Steinberg::Vst::IPlugView* PLUGIN_API createView(const Steinberg::char8* name) override;

    CzBank& get_bank() { return bank_; }
    CzMidiManager& get_midi() { return midi_; }

    void trigger_program_change(int program);
    void send_patch_to_recall(int program);
    int get_current_program() const { return current_program_.load(); }
    int get_midi_channel() const { return midi_channel_.load(); }
    void set_midi_channel(int ch);
    void connect_midi_output_by_name(const std::string& name);
    std::string get_midi_output_name() const { return current_midi_out_name_; }

    bool get_auto_send_memory_buffer() const { return auto_send_memory_buffer_.load(); }
    void set_auto_send_memory_buffer(bool enable) { auto_send_memory_buffer_.store(enable); }

    void reload_banks_from_cache();

private:
    std::atomic<uint32_t> ref_count_{1};
    Steinberg::Vst::IComponentHandler* component_handler_ = nullptr;

    std::atomic<int> current_program_{0};
    std::atomic<int> midi_channel_{1};
    std::atomic<bool> auto_send_memory_buffer_{true};

    CzBank bank_;
    CzMidiManager midi_;
    std::string current_midi_out_name_;
    std::mutex midi_mutex_;

    bool audio_active_ = false;
    int sample_size_ = 0;
};

} // namespace cz
