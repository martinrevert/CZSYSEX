#include "cz_vst_processor.h"
#include "../core/cz_sysex.h"
#include "cz_vst_view.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace cz {
extern void LogDebug(const std::string &msg);
}

namespace Steinberg {
const TUID FUnknown::iid =
    SMTG_INLINE_UID(0x00000000, 0x00000000, 0xC0000000, 0x00000046);
const TUID IPluginBase::iid =
    SMTG_INLINE_UID(0x22888DDB, 0x156E45AE, 0x8358B348, 0x08190625);
const TUID IBStream::iid =
    SMTG_INLINE_UID(0xC3BF6EA2, 0x30994752, 0x9B6BF990, 0x1EE33E9B);
const TUID IPluginFactory::iid =
    SMTG_INLINE_UID(0x7A4D811C, 0x52114A1F, 0xAED9D2EE, 0x0B43BF9F);
const TUID IPluginFactory2::iid =
    SMTG_INLINE_UID(0x0007B650, 0xF24B4C0B, 0xA464EDB9, 0xF00B2ABB);
namespace Vst {
const TUID IEventList::iid =
    SMTG_INLINE_UID(0x3A2C4214, 0x346349FE, 0xB2C4F397, 0xB9695A44);
const TUID IParamValueQueue::iid =
    SMTG_INLINE_UID(0x01263A18, 0xED074F6F, 0x98C9D356, 0x4686F9BA);
const TUID IParameterChanges::iid =
    SMTG_INLINE_UID(0xA4779663, 0x0BB64A56, 0xB44384A8, 0x466FEB9D);
const TUID IComponent::iid =
    SMTG_INLINE_UID(0xE831FF31, 0xF2D54301, 0x928EBBEE, 0x25697802);
const TUID IAudioProcessor::iid =
    SMTG_INLINE_UID(0x42043F99, 0xB7DA453C, 0xA569E79D, 0x9AAEC33D);
const TUID IComponentHandler::iid =
    SMTG_INLINE_UID(0x93A0BEA3, 0x0BD045DB, 0x8E890B0C, 0xC1E46AC6);
const TUID IPlugView::iid =
    SMTG_INLINE_UID(0x5BC32507, 0xD06049EA, 0xA6151B52, 0x2B755B29);
const TUID IEditController::iid =
    SMTG_INLINE_UID(0xDCD7BBE3, 0x7742448D, 0xA874AACC, 0x979C759E);
}
}

namespace cz {

CzVstProcessor::CzVstProcessor() {
  reload_banks_from_cache();
  auto out_devs = midi_.get_output_devices();
  if (!out_devs.empty()) {
    connect_midi_output_by_name(out_devs[0].name);
  }
}

CzVstProcessor::~CzVstProcessor() {
  if (component_handler_) {
    component_handler_->release();
    component_handler_ = nullptr;
  }
  midi_.close_output();
}

void CzVstProcessor::reload_banks_from_cache() {
  std::string p_path = CzBank::get_default_presets_cache_path();
  bank_.load_presets_cache(p_path);
  std::string m_path = CzBank::get_default_memory_cache_path();
  bank_.load_memory_cache(m_path);
}

void CzVstProcessor::connect_midi_output_by_name(const std::string &name) {
  std::lock_guard<std::mutex> lock(midi_mutex_);
  auto out_devs = midi_.get_output_devices();
  for (const auto &dev : out_devs) {
    if (dev.name == name) {
      midi_.open_output(dev.id);
      current_midi_out_name_ = name;
      return;
    }
  }
  if (!out_devs.empty()) {
    midi_.open_output(out_devs[0].id);
    current_midi_out_name_ = out_devs[0].name;
  }
}

void CzVstProcessor::set_midi_channel(int ch) {
  midi_channel_.store(std::clamp(ch, 1, 16));
}

namespace {
thread_local bool t_suppress_host_notify = false;

struct ScopedSuppressHostNotify {
  bool prev;
  ScopedSuppressHostNotify() : prev(t_suppress_host_notify) {
    t_suppress_host_notify = true;
  }
  ~ScopedSuppressHostNotify() { t_suppress_host_notify = prev; }
};
} // namespace

void CzVstProcessor::trigger_program_change(int program) {
  int p = std::clamp(program, 0, 63);
  current_program_.store(p);

  int ch = midi_channel_.load();
  {
    std::lock_guard<std::mutex> lock(midi_mutex_);
    if (midi_.is_output_open()) {
      midi_.send_program_change(static_cast<uint8_t>(p),
                                static_cast<uint8_t>(ch));
    }
  }

    if (!t_suppress_host_notify && component_handler_) {
      Steinberg::Vst::ParamValue norm = static_cast<double>(p) / 63.0;
      component_handler_->beginEdit(kParamProgram);
      component_handler_->performEdit(kParamProgram, norm);
      component_handler_->endEdit(kParamProgram);
    }

    // Auto-send memory patches to the synth's COMPARE/RECALL buffer (0x60)
    // so sound is immediately generated without requiring physical synth RAM
    // writes!
    if (auto_send_memory_buffer_.load() && p >= 32) {
      send_patch_to_recall(p);
    }
  }

  void CzVstProcessor::send_patch_to_recall(int program) {
    int p = std::clamp(program, 0, 63);
    BankMode mode = (p < 32) ? BankMode::Preset : BankMode::Memory;
    size_t idx = static_cast<size_t>(p % 32);
    CzPatch patch = bank_.get_patch_by_index(mode, idx);
    patch.midi_channel = static_cast<uint8_t>(midi_channel_.load());
    patch.memory.location = "recall";

    auto syx = CzSysEx::patch_to_sysex(patch, true);
    std::lock_guard<std::mutex> lock(midi_mutex_);
    if (midi_.is_output_open()) {
      midi_.send_sysex(syx);
    }
  }

  // FUnknown
  Steinberg::tresult PLUGIN_API CzVstProcessor::queryInterface(
      const Steinberg::TUID _iid, void **obj) {
    if (!_iid || !obj)
      return Steinberg::kInvalidArgument;

    if (memcmp(_iid, Steinberg::FUnknown::iid, 16) == 0 ||
        memcmp(_iid, Steinberg::IPluginBase::iid, 16) == 0 ||
        memcmp(_iid, Steinberg::Vst::IComponent::iid, 16) == 0) {
      *obj = static_cast<Steinberg::Vst::IComponent *>(this);
      addRef();
      return Steinberg::kResultOk;
    }
    if (memcmp(_iid, Steinberg::Vst::IAudioProcessor::iid, 16) == 0) {
      *obj = static_cast<Steinberg::Vst::IAudioProcessor *>(this);
      addRef();
      return Steinberg::kResultOk;
    }
    if (memcmp(_iid, Steinberg::Vst::IEditController::iid, 16) == 0) {
      *obj = static_cast<Steinberg::Vst::IEditController *>(this);
      addRef();
      return Steinberg::kResultOk;
    }
    *obj = nullptr;
    return Steinberg::kNoInterface;
  }

  Steinberg::uint32 PLUGIN_API CzVstProcessor::addRef() { return ++ref_count_; }

  Steinberg::uint32 PLUGIN_API CzVstProcessor::release() {
    uint32_t r = --ref_count_;
    if (r == 0) {
      delete this;
    }
    return r;
  }

  // IPluginBase
  Steinberg::tresult PLUGIN_API CzVstProcessor::initialize(
      Steinberg::FUnknown * /*context*/) {
    LogDebug("CzVstProcessor::initialize() called");
    return Steinberg::kResultOk;
  }

  Steinberg::tresult PLUGIN_API CzVstProcessor::terminate() {
    LogDebug("CzVstProcessor::terminate() called");
    if (component_handler_) {
      component_handler_->release();
      component_handler_ = nullptr;
    }
    return Steinberg::kResultOk;
  }

  // IComponent
  Steinberg::tresult PLUGIN_API CzVstProcessor::getControllerClassId(
      Steinberg::TUID /*classId*/) {
    // Single Component Plugin pattern: return kNotImplemented so the host
    // queries IEditController directly on this instance.
    LogDebug(
        "CzVstProcessor::getControllerClassId() returning kNotImplemented");
    return Steinberg::kNotImplemented;
  }

  Steinberg::tresult PLUGIN_API CzVstProcessor::setIoMode(
      Steinberg::Vst::IoMode /*mode*/) {
    return Steinberg::kResultOk;
  }

  Steinberg::int32 PLUGIN_API CzVstProcessor::getBusCount(
      Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir) {
    if (type == Steinberg::Vst::kAudio) {
      return (dir == Steinberg::Vst::kOutput) ? 1 : 0;
    }
    if (type == Steinberg::Vst::kEvent) {
      return 1;
    }
    return 0;
  }

  static void ascii_to_utf16(const char *src, Steinberg::char16 *dst,
                             size_t maxLen) {
    size_t i = 0;
    while (src[i] && i < maxLen - 1) {
      dst[i] = static_cast<Steinberg::char16>(src[i]);
      ++i;
    }
    dst[i] = 0;
  }

  Steinberg::tresult PLUGIN_API CzVstProcessor::getBusInfo(
      Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir,
      Steinberg::int32 index, Steinberg::Vst::BusInfo & bus) {
    char dbg[128];
    snprintf(dbg, sizeof(dbg),
             "CzVstProcessor::getBusInfo(type=%d, dir=%d, idx=%d)", type, dir,
             index);
    LogDebug(dbg);
    if (index != 0)
      return Steinberg::kInvalidArgument;
    memset(&bus, 0, sizeof(bus));
    bus.mediaType = type;
    bus.direction = dir;
    bus.busType = Steinberg::Vst::kMain;
    bus.flags = 1; // kDefaultActive

    if (type == Steinberg::Vst::kAudio && dir == Steinberg::Vst::kOutput) {
      bus.channelCount = 2; // Stereo
      ascii_to_utf16("CZ Audio Out", bus.name, 128);
      return Steinberg::kResultOk;
    }
    if (type == Steinberg::Vst::kEvent) {
      bus.channelCount = 16;
      if (dir == Steinberg::Vst::kInput) {
        ascii_to_utf16("MIDI In", bus.name, 128);
      } else {
        ascii_to_utf16("MIDI Out", bus.name, 128);
      }
      return Steinberg::kResultOk;
    }
    return Steinberg::kInvalidArgument;
  }

  Steinberg::tresult PLUGIN_API CzVstProcessor::getRoutingInfo(
      Steinberg::Vst::RoutingInfo & inInfo,
      Steinberg::Vst::RoutingInfo & outInfo) {
    if (inInfo.mediaType == Steinberg::Vst::kEvent && inInfo.busIndex == 0) {
      outInfo.mediaType = Steinberg::Vst::kAudio;
      outInfo.busIndex = 0;
      outInfo.channel = 0;
      return Steinberg::kResultOk;
    }
    return Steinberg::kNotImplemented;
  }

  Steinberg::tresult PLUGIN_API CzVstProcessor::activateBus(
      Steinberg::Vst::MediaType /*type*/, Steinberg::Vst::BusDirection /*dir*/,
      Steinberg::int32 /*index*/, Steinberg::TBool /*state*/) {
    return Steinberg::kResultOk;
  }

  Steinberg::tresult PLUGIN_API CzVstProcessor::setActive(
      Steinberg::TBool state) {
    LogDebug(std::string("CzVstProcessor::setActive(") +
             (state ? "true" : "false") + ")");
    audio_active_ = (state != 0);
    return Steinberg::kResultOk;
  }

  struct CzVstSaveState {
    uint32_t magic = 0x435A334B; // 'CZ3K'
    uint32_t version = 2;
    int32_t program = 0;
    int32_t midi_channel = 1;
    char midi_out_name[64] = {0};
    int32_t auto_send_buffer = 1;
  };

  Steinberg::tresult PLUGIN_API CzVstProcessor::setState(Steinberg::IBStream *
                                                         state) {
    if (!state)
      return Steinberg::kInvalidArgument;
    CzVstSaveState data{};
    Steinberg::int32 bytesRead = 0;
      Steinberg::tresult res = state->read(&data, sizeof(data), &bytesRead);
      char dbg[128];
      snprintf(dbg, sizeof(dbg),
               "CzVstProcessor::setState(res=%d, bytesRead=%d)", res,
               bytesRead);
      LogDebug(dbg);
      if (res == Steinberg::kResultOk && bytesRead == 0) {
        return Steinberg::kResultOk;
      }
      if (res == Steinberg::kResultOk && bytesRead == sizeof(data)) {
        if (data.magic == 0x435A334B) {
          ScopedSuppressHostNotify guard;
          current_program_.store(std::clamp((int)data.program, 0, 63));
          midi_channel_.store(std::clamp((int)data.midi_channel, 1, 16));
          if (data.version >= 2) {
            auto_send_memory_buffer_.store(data.auto_send_buffer != 0);
          }
          std::string dev_name(data.midi_out_name);
          if (!dev_name.empty()) {
            connect_midi_output_by_name(dev_name);
          }
          trigger_program_change(current_program_.load());
          return Steinberg::kResultOk;
        }
      }
      return Steinberg::kResultFalse;
    }

    Steinberg::tresult PLUGIN_API CzVstProcessor::getState(Steinberg::IBStream *
                                                           state) {
      LogDebug("CzVstProcessor::getState() called");
      if (!state)
        return Steinberg::kInvalidArgument;
      CzVstSaveState data{};
      data.version = 2;
      data.program = current_program_.load();
      data.midi_channel = midi_channel_.load();
      data.auto_send_buffer = auto_send_memory_buffer_.load() ? 1 : 0;
      strncpy_s(data.midi_out_name, current_midi_out_name_.c_str(),
                sizeof(data.midi_out_name) - 1);

      Steinberg::int32 bytesWritten = 0;
      return state->write(&data, sizeof(data), &bytesWritten);
    }

    // IAudioProcessor
    Steinberg::tresult PLUGIN_API CzVstProcessor::setBusArrangements(
        Steinberg::Vst::SpeakerArrangement * inputs, Steinberg::int32 numIns,
        Steinberg::Vst::SpeakerArrangement * outputs,
        Steinberg::int32 numOuts) {
      char dbg[160];
      snprintf(
          dbg, sizeof(dbg),
          "CzVstProcessor::setBusArrangements(numIns=%d, in0=%llu, numOuts=%d, "
          "out0=%llu)",
          numIns, (numIns > 0 && inputs) ? (unsigned long long)inputs[0] : 0ULL,
          numOuts,
          (numOuts > 0 && outputs) ? (unsigned long long)outputs[0] : 0ULL);
      LogDebug(dbg);
      if (numIns > 0 && inputs) {
        // We are an instrument with 0 audio inputs
        if (inputs[0] != Steinberg::Vst::SpeakerArr::kEmpty) {
          return Steinberg::kResultFalse;
        }
      }
        if (numOuts == 1 && outputs) {
          if (outputs[0] == Steinberg::Vst::SpeakerArr::kStereo) {
            return Steinberg::kResultOk;
          }
          outputs[0] = Steinberg::Vst::SpeakerArr::kStereo;
          return Steinberg::kResultFalse;
        }
        if (numOuts == 0) {
          return Steinberg::kResultOk;
        }
        return Steinberg::kResultFalse;
      }

      Steinberg::tresult PLUGIN_API CzVstProcessor::getBusArrangement(
          Steinberg::Vst::BusDirection dir, Steinberg::int32 index,
          Steinberg::Vst::SpeakerArrangement & arr) {
        if (index != 0)
          return Steinberg::kInvalidArgument;
        if (dir == Steinberg::Vst::kOutput) {
          arr = Steinberg::Vst::SpeakerArr::kStereo;
          return Steinberg::kResultOk;
        }
        arr = Steinberg::Vst::SpeakerArr::kEmpty;
        return Steinberg::kResultOk;
      }

      Steinberg::tresult PLUGIN_API CzVstProcessor::canProcessSampleSize(
          Steinberg::int32 symbolicSampleSize) {
        char dbg[96];
        snprintf(dbg, sizeof(dbg), "CzVstProcessor::canProcessSampleSize(%d)",
                 symbolicSampleSize);
        LogDebug(dbg);
        // 0 = kSample32 (32-bit float), 1 = kSample64 (64-bit float)
        return (symbolicSampleSize == 0 || symbolicSampleSize == 1)
                   ? Steinberg::kResultOk
                   : Steinberg::kResultFalse;
      }

      Steinberg::uint32 PLUGIN_API CzVstProcessor::getLatencySamples() {
        return 0;
      }

      Steinberg::tresult PLUGIN_API CzVstProcessor::setupProcessing(
          Steinberg::Vst::ProcessSetup & setup) {
        char dbg[128];
        snprintf(dbg, sizeof(dbg),
                 "CzVstProcessor::setupProcessing(mode=%d, sym=%d, maxBlk=%d, "
                 "sr=%.1f)",
                 setup.processMode, setup.symbolicSampleSize,
                 setup.maxSamplesPerBlock, setup.sampleRate);
        LogDebug(dbg);
        sample_size_ = setup.symbolicSampleSize;
        return Steinberg::kResultOk;
      }

      Steinberg::tresult PLUGIN_API CzVstProcessor::setProcessing(
          Steinberg::TBool /*state*/) {
        return Steinberg::kResultOk;
      }

      Steinberg::tresult PLUGIN_API CzVstProcessor::process(
          Steinberg::Vst::ProcessData & data) {
        ScopedSuppressHostNotify guard;

        // 1. Process parameter changes
        if (data.inputParameterChanges) {
          int32_t numParams = data.inputParameterChanges->getParameterCount();
          for (int32_t i = 0; i < numParams; ++i) {
            auto *queue = data.inputParameterChanges->getParameterData(i);
            if (!queue)
              continue;
            Steinberg::Vst::ParamID pid = queue->getParameterId();
            int32_t numPoints = queue->getPointCount();
            if (numPoints > 0) {
              int32_t offset = 0;
              Steinberg::Vst::ParamValue val = 0.0;
              if (queue->getPoint(numPoints - 1, offset, val) ==
                  Steinberg::kResultOk) {
                if (pid == kParamProgram) {
                  int p = std::clamp(static_cast<int>(val * 63.0 + 0.5), 0, 63);
                  if (p != current_program_.load()) {
                    trigger_program_change(p);
                  }
                } else if (pid == kParamMidiChannel) {
                  int ch =
                      std::clamp(static_cast<int>(val * 15.0 + 1.5), 1, 16);
                  if (ch != midi_channel_.load()) {
                    set_midi_channel(ch);
                  }
                }
              }
            }
          }
        }

        // 2. Direct MIDI Hardware Thru & Event Forwarding
        if (data.inputEvents) {
          int32_t count = data.inputEvents->getEventCount();
          for (int32_t i = 0; i < count; ++i) {
            Steinberg::Vst::Event e{};
            if (data.inputEvents->getEvent(i, e) == Steinberg::kResultOk) {
              if (data.outputEvents) {
                data.outputEvents->addEvent(e);
              }

              int ch = midi_channel_.load();
              if (e.type == Steinberg::Vst::kNoteOnEvent) {
                uint8_t note = static_cast<uint8_t>(e.noteOn.pitch);
                uint8_t vel = static_cast<uint8_t>(
                    std::clamp(e.noteOn.velocity * 127.0f, 0.0f, 127.0f));
                std::lock_guard<std::mutex> lock(midi_mutex_);
                if (midi_.is_output_open()) {
                  if (vel > 0) {
                    midi_.send_note_on(note, vel, static_cast<uint8_t>(ch));
                  } else {
                    midi_.send_note_off(note, static_cast<uint8_t>(ch));
                  }
                }
              } else if (e.type == Steinberg::Vst::kNoteOffEvent) {
                uint8_t note = static_cast<uint8_t>(e.noteOff.pitch);
                std::lock_guard<std::mutex> lock(midi_mutex_);
                if (midi_.is_output_open()) {
                  midi_.send_note_off(note, static_cast<uint8_t>(ch));
                }
              }
            }
          }
        }

                  // 3. Clear all audio output buffers (supporting both 32-bit
                  // and 64-bit float engines)
                  const bool is_64 = (data.symbolicSampleSize == 1);
                  if (data.numOutputs > 0 && data.outputs) {
                    for (int32_t b = 0; b < data.numOutputs; ++b) {
                      auto &bus = data.outputs[b];
                      if (data.numSamples > 0) {
                        if (is_64) {
                          if (bus.channelBuffers64) {
                            for (int32_t c = 0; c < bus.numChannels; ++c) {
                              if (bus.channelBuffers64[c]) {
                                memset(bus.channelBuffers64[c], 0,
                                       static_cast<size_t>(data.numSamples) *
                                           sizeof(double));
                              }
                            }
                          }
                          } else {
                            if (bus.channelBuffers32) {
                              for (int32_t c = 0; c < bus.numChannels; ++c) {
                                if (bus.channelBuffers32[c]) {
                                  memset(bus.channelBuffers32[c], 0,
                                         static_cast<size_t>(data.numSamples) *
                                             sizeof(float));
                                }
                              }
                            }
                          }
                        }
                        if (bus.numChannels >= 64) {
                          bus.silenceFlags = ~0ULL;
                        } else if (bus.numChannels > 0) {
                          bus.silenceFlags = (1ULL << bus.numChannels) - 1ULL;
                        } else {
                          bus.silenceFlags = 0ULL;
                        }
                      }
                    }

                    return Steinberg::kResultOk;
                  }

                  Steinberg::uint32 PLUGIN_API
                  CzVstProcessor::getTailSamples() {
                    return 0;
                  }

                  // IEditController
                  Steinberg::tresult PLUGIN_API
                  CzVstProcessor::setComponentState(Steinberg::IBStream *
                                                    state) {
                    LogDebug("CzVstProcessor::setComponentState() called");
                    if (!state)
                      return Steinberg::kResultOk;
                    return setState(state);
                  }

                  Steinberg::int32 PLUGIN_API
                  CzVstProcessor::getParameterCount() {
                    return kParamCount;
                  }

                  Steinberg::tresult PLUGIN_API
                  CzVstProcessor::getParameterInfo(
                      Steinberg::int32 paramIndex,
                      Steinberg::Vst::ParameterInfo & info) {
                    memset(&info, 0, sizeof(info));
                    if (paramIndex == kParamProgram) {
                      info.id = kParamProgram;
                      ascii_to_utf16("Patch Select", info.title, 128);
                      ascii_to_utf16("Patch", info.shortTitle, 128);
                      ascii_to_utf16("", info.units, 128);
                      info.stepCount = 63;
                      info.defaultNormalizedValue = 0.0;
                      info.unitId = 0;
                      info.flags = Steinberg::Vst::kCanAutomate |
                                   Steinberg::Vst::kIsList |
                                   Steinberg::Vst::kIsProgramChange;
                      return Steinberg::kResultOk;
                    }
                    if (paramIndex == kParamMidiChannel) {
                      info.id = kParamMidiChannel;
                      ascii_to_utf16("MIDI Channel", info.title, 128);
                      ascii_to_utf16("Ch", info.shortTitle, 128);
                      ascii_to_utf16("", info.units, 128);
                      info.stepCount = 15;
                      info.defaultNormalizedValue = 0.0;
                      info.unitId = 0;
                      info.flags = Steinberg::Vst::kCanAutomate;
                      return Steinberg::kResultOk;
                    }
                    return Steinberg::kInvalidArgument;
                  }

                  Steinberg::tresult PLUGIN_API
                  CzVstProcessor::getParamStringByValue(
                      Steinberg::Vst::ParamID id,
                      Steinberg::Vst::ParamValue valueNormalized,
                      Steinberg::char16 * string) {
                    if (!string)
                      return Steinberg::kInvalidArgument;
                    if (id == kParamProgram) {
                      int p = std::clamp(
                          static_cast<int>(valueNormalized * 63.0 + 0.5), 0,
                          63);
                      BankMode m =
                          (p < 32) ? BankMode::Preset : BankMode::Memory;
                      size_t idx = static_cast<size_t>(p % 32);
                      auto [b, s] = CzBank::get_bank_slot(idx);
                      const auto &patch = bank_.get_patch_by_index(m, idx);

                      char buf[128];
                      snprintf(buf, sizeof(buf), "%s %c-%d: %s",
                               (p < 32 ? "PRESET" : "MEMORY"), b, s,
                               patch.name.c_str());
                      ascii_to_utf16(buf, string, 128);
                      return Steinberg::kResultOk;
                    }
                    if (id == kParamMidiChannel) {
                      int ch = std::clamp(
                          static_cast<int>(valueNormalized * 15.0 + 1.5), 1,
                          16);
                      char buf[32];
                      snprintf(buf, sizeof(buf), "Ch %d", ch);
                      ascii_to_utf16(buf, string, 128);
                      return Steinberg::kResultOk;
                    }
                    return Steinberg::kInvalidArgument;
                  }

                  Steinberg::tresult PLUGIN_API
                  CzVstProcessor::getParamValueByString(
                      Steinberg::Vst::ParamID /*id*/,
                      const Steinberg::char16 * /*string*/,
                      Steinberg::Vst::ParamValue & /*valueNormalized*/) {
                    return Steinberg::kNotImplemented;
                  }

                  Steinberg::Vst::ParamValue PLUGIN_API
                  CzVstProcessor::normalizedParamToPlain(
                      Steinberg::Vst::ParamID id,
                      Steinberg::Vst::ParamValue valueNormalized) {
                    if (id == kParamProgram)
                      return valueNormalized * 63.0;
                    if (id == kParamMidiChannel)
                      return valueNormalized * 15.0 + 1.0;
                    return valueNormalized;
                  }

                  Steinberg::Vst::ParamValue PLUGIN_API
                  CzVstProcessor::plainParamToNormalized(
                      Steinberg::Vst::ParamID id,
                      Steinberg::Vst::ParamValue plainValue) {
                    if (id == kParamProgram)
                      return plainValue / 63.0;
                    if (id == kParamMidiChannel)
                      return (plainValue - 1.0) / 15.0;
                    return plainValue;
                  }

                  Steinberg::Vst::ParamValue PLUGIN_API
                  CzVstProcessor::getParamNormalized(
                      Steinberg::Vst::ParamID id) {
                    if (id == kParamProgram)
                      return static_cast<double>(current_program_.load()) /
                             63.0;
                    if (id == kParamMidiChannel)
                      return static_cast<double>(midi_channel_.load() - 1) /
                             15.0;
                    return 0.0;
                  }

                  Steinberg::tresult PLUGIN_API
                  CzVstProcessor::setParamNormalized(
                      Steinberg::Vst::ParamID id,
                      Steinberg::Vst::ParamValue value) {
                    ScopedSuppressHostNotify guard;
                    if (id == kParamProgram) {
                      int p = std::clamp(static_cast<int>(value * 63.0 + 0.5),
                                         0, 63);
                      if (p != current_program_.load()) {
                        trigger_program_change(p);
                      }
                      return Steinberg::kResultOk;
                    }
                    if (id == kParamMidiChannel) {
                      int ch = std::clamp(static_cast<int>(value * 15.0 + 1.5),
                                          1, 16);
                      set_midi_channel(ch);
                      return Steinberg::kResultOk;
                    }
                    return Steinberg::kInvalidArgument;
                  }

                  Steinberg::tresult PLUGIN_API
                  CzVstProcessor::setComponentHandler(
                      Steinberg::Vst::IComponentHandler * handler) {
                    LogDebug("CzVstProcessor::setComponentHandler called");
                    if (component_handler_ != handler) {
                      if (component_handler_) {
                        component_handler_->release();
                      }
                      component_handler_ = handler;
                      if (component_handler_) {
                        component_handler_->addRef();
                      }
                    }
                    return Steinberg::kResultOk;
                  }

                  Steinberg::Vst::IPlugView *PLUGIN_API
                  CzVstProcessor::createView(const Steinberg::char8 *name) {
                    LogDebug(
                        std::string(
                            "CzVstProcessor::createView called with name: ") +
                        (name ? name : "null"));
                    if (name && strcmp(name, "editor") == 0) {
                      return new CzVstView(*this);
                    }
                    return nullptr;
                  }

                } // namespace cz
