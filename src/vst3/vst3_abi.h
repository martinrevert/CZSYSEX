#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <windows.h>

#if _WIN32
#define SMTG_STDMETHODCALLTYPE __stdcall
#define SMTG_COM_COMPATIBLE 1
#else
#define SMTG_STDMETHODCALLTYPE
#define SMTG_COM_COMPATIBLE 0
#endif

#define PLUGIN_API SMTG_STDMETHODCALLTYPE

namespace Steinberg {

typedef int32_t tresult;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int8_t int8;
typedef uint8_t uint8;
typedef int64_t int64;
typedef uint64_t uint64;
typedef char char8;
typedef char16_t char16;
typedef double ParamValue;
typedef uint32 ParamID;
typedef uint8 TBool;

#if SMTG_COM_COMPATIBLE
static constexpr tresult kNoInterface =
    static_cast<tresult>(0x80004002L); // E_NOINTERFACE
static constexpr tresult kResultOk = static_cast<tresult>(0x00000000L); // S_OK
static constexpr tresult kResultTrue = kResultOk;
static constexpr tresult kResultFalse =
    static_cast<tresult>(0x00000001L); // S_FALSE
static constexpr tresult kInvalidArgument =
    static_cast<tresult>(0x80070057L); // E_INVALIDARG
static constexpr tresult kNotImplemented =
    static_cast<tresult>(0x80004001L); // E_NOTIMPL
static constexpr tresult kInternalError =
    static_cast<tresult>(0x80004005L); // E_FAIL
static constexpr tresult kNotInitialized =
    static_cast<tresult>(0x8000FFFFL); // E_UNEXPECTED
static constexpr tresult kOutOfMemory =
    static_cast<tresult>(0x8007000EL); // E_OUTOFMEMORY
#else
enum {
  kNoInterface = -1,
  kResultOk = 0,
  kResultTrue = kResultOk,
  kResultFalse = 1,
  kInvalidArgument = 2,
  kNotImplemented = 3,
  kInternalError = 4,
  kNotInitialized = 5,
  kOutOfMemory = 6
};
#endif

typedef char8 TUID[16];

#if SMTG_COM_COMPATIBLE
#define SMTG_INLINE_UID(l1, l2, l3, l4)                                        \
  {(char)(((uint32_t)(l1) & 0x000000FF)),                                      \
   (char)(((uint32_t)(l1) & 0x0000FF00) >> 8),                                 \
   (char)(((uint32_t)(l1) & 0x00FF0000) >> 16),                                \
   (char)(((uint32_t)(l1) & 0xFF000000) >> 24),                                \
   (char)(((uint32_t)(l2) & 0x00FF0000) >> 16),                                \
   (char)(((uint32_t)(l2) & 0xFF000000) >> 24),                                \
   (char)(((uint32_t)(l2) & 0x000000FF)),                                      \
   (char)(((uint32_t)(l2) & 0x0000FF00) >> 8),                                 \
   (char)(((uint32_t)(l3) & 0xFF000000) >> 24),                                \
   (char)(((uint32_t)(l3) & 0x00FF0000) >> 16),                                \
   (char)(((uint32_t)(l3) & 0x0000FF00) >> 8),                                 \
   (char)(((uint32_t)(l3) & 0x000000FF)),                                      \
   (char)(((uint32_t)(l4) & 0xFF000000) >> 24),                                \
   (char)(((uint32_t)(l4) & 0x00FF0000) >> 16),                                \
   (char)(((uint32_t)(l4) & 0x0000FF00) >> 8),                                 \
   (char)(((uint32_t)(l4) & 0x000000FF))}
#else
#define SMTG_INLINE_UID(l1, l2, l3, l4)                                        \
  {                                                                            \
      (char)(((uint32_t)(l1) >> 24) & 0xFF),                                   \
      (char)(((uint32_t)(l1) >> 16) & 0xFF),                                   \
      (char)(((uint32_t)(l1) >> 8) & 0xFF),                                    \
      (char)((uint32_t)(l1) & 0xFF),                                           \
      (char)(((uint32_t)(l2) >> 24) & 0xFF),                                   \
      (char)(((uint32_t)(l2) >> 16) & 0xFF),                                   \
      (char)(((uint32_t)(l2) >> 8) & 0xFF),                                    \
      (char)((uint32_t)(l2) & 0xFF),                                           \
      (char)(((uint32_t)(l3) >> 24) & 0xFF),                                   \
      (char)(((uint32_t)(l3) >> 16) & 0xFF),                                   \
      (char)(((uint32_t)(l3) >> 8) & 0xFF),                                    \
      (char)((uint32_t)(l3) & 0xFF),                                           \
      (char)(((uint32_t)(l4) >> 24) & 0xFF),                                   \
      (char)(((uint32_t)(l4) >> 16) & 0xFF),                                   \
      (char)(((uint32_t)(l4) >> 8) & 0xFF),                                    \
      (char)((uint32_t)(l4) & 0xFF)}
#endif

#define INLINE_UID SMTG_INLINE_UID

struct FUnknown {
  virtual tresult PLUGIN_API queryInterface(const TUID _iid, void **obj) = 0;
  virtual uint32 PLUGIN_API addRef() = 0;
  virtual uint32 PLUGIN_API release() = 0;
  static const TUID iid;
};

struct IPluginBase : public FUnknown {
  virtual tresult PLUGIN_API initialize(FUnknown *context) = 0;
  virtual tresult PLUGIN_API terminate() = 0;
  static const TUID iid;
};

struct IBStream : public FUnknown {
  virtual tresult PLUGIN_API read(void *buffer, int32 numBytes,
                                  int32 *numBytesRead = nullptr) = 0;
  virtual tresult PLUGIN_API write(void *buffer, int32 numBytes,
                                   int32 *numBytesWritten = nullptr) = 0;
  virtual tresult PLUGIN_API seek(int64 pos, int32 mode,
                                  int64 *result = nullptr) = 0;
  virtual tresult PLUGIN_API tell(int64 *pos) = 0;
  static const TUID iid;
};

struct PFactoryInfo {
  enum FactoryFlags {
    kNoFlags = 0,
    kClassesDiscardable = 1 << 0,
    kLicenseCheck = 1 << 1,
    kComponentNonDiscardable = 1 << 3,
    kUnicode = 1 << 4
  };
  char8 vendor[64];
  char8 url[256];
  char8 email[128];
  int32 flags;
};

struct PClassInfo2 {
  TUID cid;
  int32 cardinality;
  char8 category[32];
  char8 name[64];
  int32 classFlags;
  char8 subCategories[128];
  char8 vendor[64];
  char8 version[64];
  char8 sdkVersion[64];
};

struct IPluginFactory : public FUnknown {
  virtual tresult PLUGIN_API getFactoryInfo(PFactoryInfo *info) = 0;
  virtual int32 PLUGIN_API countClasses() = 0;
  virtual tresult PLUGIN_API getClassInfo(int32 index, void *info) = 0;
  virtual tresult PLUGIN_API createInstance(const TUID cid, const TUID _iid,
                                            void **obj) = 0;
  static const TUID iid;
};

struct IPluginFactory2 : public IPluginFactory {
  virtual tresult PLUGIN_API getClassInfo2(int32 index, PClassInfo2 *info) = 0;
  static const TUID iid;
};

namespace Vst {

typedef Steinberg::ParamValue ParamValue;
typedef Steinberg::ParamID ParamID;

typedef int32 MediaType;
typedef int32 BusDirection;
typedef int32 BusType;
typedef int32 IoMode;
typedef uint64 SpeakerArrangement;

enum MediaTypes { kAudio = 0, kEvent };
enum BusDirections { kInput = 0, kOutput };
enum BusTypes { kMain = 0, kAux };

namespace SpeakerArr {
const SpeakerArrangement kEmpty = 0;
const SpeakerArrangement kMono = 1ULL << 19;                  // kSpeakerM
const SpeakerArrangement kStereo = (1ULL << 0) | (1ULL << 1); // L + R = 3
} // namespace SpeakerArr

struct BusInfo {
  MediaType mediaType;
  BusDirection direction;
  int32 channelCount;
  char16 name[128];
  BusType busType;
  uint32 flags;
};

struct RoutingInfo {
  MediaType mediaType;
  int32 busIndex;
  int32 channel;
};

struct ProcessSetup {
  int32 processMode;
  int32 symbolicSampleSize;
  int32 maxSamplesPerBlock;
  double sampleRate;
};

struct AudioBusBuffers {
  int32 numChannels;
  uint64 silenceFlags;
  union {
    float **channelBuffers32;
    double **channelBuffers64;
  };
};

struct Event {
  int32 busIndex;
  int32 sampleOffset;
  double ppqPosition;
  uint16 flags;
  uint16 type;
  union {
    struct {
      int16 channel;
      int16 pitch;
      float tuning;
      float velocity;
      int32 length;
      int32 noteId;
    } noteOn;
    struct {
      int16 channel;
      int16 pitch;
      float velocity;
      int32 noteId;
      float tuning;
    } noteOff;
    struct {
    uint32 size;
    uint32 type;
    const uint8 *bytes;
  } data;
  struct {
    uint32 typeId;
    int32 noteId;
    uint32 textLen;
    const char16 *text;
  } noteExpressionText;
  struct {
    uint8 controlNumber;
    int8 value;
    int8 value2;
  } midiCCOut;
  uint8 raw[24];
};
};

enum EventTypes {
  kNoteOnEvent = 0,
  kNoteOffEvent,
  kDataEvent,
  kPolyPressureEvent,
  kNoteExpressionValueEvent,
  kNoteExpressionTextEvent,
  kChordEvent,
  kScaleEvent,
  kLegacyMIDICCOutEvent = 65535
};

struct IEventList : public FUnknown {
  virtual int32 PLUGIN_API getEventCount() = 0;
  virtual tresult PLUGIN_API getEvent(int32 index, Event &e) = 0;
  virtual tresult PLUGIN_API addEvent(Event &e) = 0;
  static const TUID iid;
};

struct IParamValueQueue : public FUnknown {
  virtual ParamID PLUGIN_API getParameterId() = 0;
  virtual int32 PLUGIN_API getPointCount() = 0;
  virtual tresult PLUGIN_API getPoint(int32 index, int32 &sampleOffset,
                                      ParamValue &value) = 0;
  virtual tresult PLUGIN_API addPoint(int32 sampleOffset, ParamValue value,
                                      int32 &index) = 0;
  static const TUID iid;
};

struct IParameterChanges : public FUnknown {
  virtual int32 PLUGIN_API getParameterCount() = 0;
  virtual IParamValueQueue *PLUGIN_API getParameterData(int32 index) = 0;
  virtual IParamValueQueue *PLUGIN_API addParameterData(const ParamID &id,
                                                        int32 &index) = 0;
  static const TUID iid;
};

struct ProcessData {
  int32 processMode;
  int32 symbolicSampleSize;
  int32 numSamples;
  int32 numInputs;
  int32 numOutputs;
  AudioBusBuffers *inputs;
  AudioBusBuffers *outputs;
  IParameterChanges *inputParameterChanges;
  IParameterChanges *outputParameterChanges;
  IEventList *inputEvents;
  IEventList *outputEvents;
  void *processContext;
};

struct IComponent : public IPluginBase {
  virtual tresult PLUGIN_API getControllerClassId(TUID classId) = 0;
  virtual tresult PLUGIN_API setIoMode(IoMode mode) = 0;
  virtual int32 PLUGIN_API getBusCount(MediaType type, BusDirection dir) = 0;
  virtual tresult PLUGIN_API getBusInfo(MediaType type, BusDirection dir,
                                        int32 index, BusInfo &bus) = 0;
  virtual tresult PLUGIN_API getRoutingInfo(RoutingInfo &inInfo,
                                            RoutingInfo &outInfo) = 0;
  virtual tresult PLUGIN_API activateBus(MediaType type, BusDirection dir,
                                         int32 index, TBool state) = 0;
  virtual tresult PLUGIN_API setActive(TBool state) = 0;
  virtual tresult PLUGIN_API setState(IBStream *state) = 0;
  virtual tresult PLUGIN_API getState(IBStream *state) = 0;
  static const TUID iid;
};

struct IAudioProcessor : public FUnknown {
  virtual tresult PLUGIN_API setBusArrangements(SpeakerArrangement *inputs,
                                                int32 numIns,
                                                SpeakerArrangement *outputs,
                                                int32 numOuts) = 0;
  virtual tresult PLUGIN_API getBusArrangement(BusDirection dir, int32 index,
                                               SpeakerArrangement &arr) = 0;
  virtual tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) = 0;
  virtual uint32 PLUGIN_API getLatencySamples() = 0;
  virtual tresult PLUGIN_API setupProcessing(ProcessSetup &setup) = 0;
  virtual tresult PLUGIN_API setProcessing(TBool state) = 0;
  virtual tresult PLUGIN_API process(ProcessData &data) = 0;
  virtual uint32 PLUGIN_API getTailSamples() = 0;
  static const TUID iid;
};

enum ParameterFlags {
  kNoFlags = 0,
  kCanAutomate = 1 << 0,
  kIsReadOnly = 1 << 1,
  kIsWrapAround = 1 << 2,
  kIsList = 1 << 3,
  kIsHidden = 1 << 4,
  kIsProgramChange = 1 << 15,
  kIsBypass = 1 << 16
};

struct ParameterInfo {
  ParamID id;
  char16 title[128];
  char16 shortTitle[128];
  char16 units[128];
  int32 stepCount;
  ParamValue defaultNormalizedValue;
  int32 unitId;
  int32 flags;
};

struct IComponentHandler : public FUnknown {
  virtual tresult PLUGIN_API beginEdit(ParamID id) = 0;
  virtual tresult PLUGIN_API performEdit(ParamID id,
                                         ParamValue valueNormalized) = 0;
  virtual tresult PLUGIN_API endEdit(ParamID id) = 0;
  virtual tresult PLUGIN_API restartComponent(int32 flags) = 0;
  static const TUID iid;
};

struct ViewRect {
  int32 left;
  int32 top;
  int32 right;
  int32 bottom;
};

struct IPlugView : public FUnknown {
  virtual tresult PLUGIN_API isPlatformTypeSupported(const char8 *type) = 0;
  virtual tresult PLUGIN_API attached(void *parent, const char8 *type) = 0;
  virtual tresult PLUGIN_API removed() = 0;
  virtual tresult PLUGIN_API onWheel(float distance) = 0;
  virtual tresult PLUGIN_API onKeyDown(char16 key, int16 keyCode,
                                       int16 modifiers) = 0;
  virtual tresult PLUGIN_API onKeyUp(char16 key, int16 keyCode,
                                     int16 modifiers) = 0;
  virtual tresult PLUGIN_API getSize(ViewRect *size) = 0;
  virtual tresult PLUGIN_API onSize(ViewRect *newSize) = 0;
  virtual tresult PLUGIN_API onFocus(TBool state) = 0;
  virtual tresult PLUGIN_API setFrame(void *frame) = 0;
  virtual tresult PLUGIN_API canResize() = 0;
  virtual tresult PLUGIN_API checkSizeConstraint(ViewRect *rect) = 0;
  static const TUID iid;
};

struct IEditController : public IPluginBase {
  virtual tresult PLUGIN_API setComponentState(IBStream *state) = 0;
  virtual tresult PLUGIN_API setState(IBStream *state) = 0;
  virtual tresult PLUGIN_API getState(IBStream *state) = 0;
  virtual int32 PLUGIN_API getParameterCount() = 0;
  virtual tresult PLUGIN_API getParameterInfo(int32 paramIndex,
                                              ParameterInfo &info) = 0;
  virtual tresult PLUGIN_API getParamStringByValue(ParamID id,
                                                   ParamValue valueNormalized,
                                                   char16 *string) = 0;
  virtual tresult PLUGIN_API getParamValueByString(
      ParamID id, const char16 *string, ParamValue &valueNormalized) = 0;
  virtual ParamValue PLUGIN_API
  normalizedParamToPlain(ParamID id, ParamValue valueNormalized) = 0;
  virtual ParamValue PLUGIN_API
  plainParamToNormalized(ParamID id, ParamValue plainValue) = 0;
  virtual ParamValue PLUGIN_API getParamNormalized(ParamID id) = 0;
  virtual tresult PLUGIN_API setParamNormalized(ParamID id,
                                                ParamValue value) = 0;
  virtual tresult PLUGIN_API
  setComponentHandler(IComponentHandler *handler) = 0;
  virtual IPlugView *PLUGIN_API createView(const char8 *name) = 0;
  static const TUID iid;
};

} // namespace Vst

#if defined(_WIN64) || defined(_M_X64) || defined(__x86_64__)
static_assert(sizeof(PFactoryInfo) == 452, "PFactoryInfo ABI size mismatch");
static_assert(sizeof(PClassInfo2) == 440, "PClassInfo2 ABI size mismatch");
static_assert(sizeof(Vst::BusInfo) == 276, "BusInfo ABI size mismatch");
static_assert(sizeof(Vst::RoutingInfo) == 12, "RoutingInfo ABI size mismatch");
static_assert(sizeof(Vst::ProcessSetup) == 24,
              "ProcessSetup ABI size mismatch");
static_assert(sizeof(Vst::AudioBusBuffers) == 24,
              "AudioBusBuffers ABI size mismatch");
static_assert(sizeof(Vst::Event) == 48, "Event ABI size mismatch");
static_assert(offsetof(Vst::Event, noteOn) == 24,
              "Event::noteOn ABI offset mismatch");
static_assert(sizeof(Vst::ProcessData) == 80, "ProcessData ABI size mismatch");
static_assert(offsetof(Vst::ProcessData, symbolicSampleSize) == 4,
              "ProcessData::symbolicSampleSize ABI offset mismatch");
static_assert(offsetof(Vst::ProcessData, outputs) == 32,
              "ProcessData::outputs ABI offset mismatch");
static_assert(offsetof(Vst::ProcessData, inputParameterChanges) == 40,
              "ProcessData::inputParameterChanges ABI offset mismatch");
static_assert(offsetof(Vst::ProcessData, inputEvents) == 56,
              "ProcessData::inputEvents ABI offset mismatch");
static_assert(sizeof(Vst::ParameterInfo) == 792,
              "ParameterInfo ABI size mismatch");
static_assert(sizeof(Vst::ViewRect) == 16, "ViewRect ABI size mismatch");
#endif

} // namespace Steinberg
