#include "cz_vst_processor.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <windows.h>

namespace cz {

void LogDebug(const std::string &msg) {
  char path[MAX_PATH];
  if (GetTempPathA(MAX_PATH, path)) {
    std::string logfile = std::string(path) + "cz3000_vst3.log";
    std::ofstream out(logfile, std::ios::app);
    if (out) {
      SYSTEMTIME st;
      GetLocalTime(&st);
      out << "[" << std::setfill('0') << std::setw(2) << st.wHour << ":"
          << std::setfill('0') << std::setw(2) << st.wMinute << ":"
          << std::setfill('0') << std::setw(2) << st.wSecond << "."
          << std::setfill('0') << std::setw(3) << st.wMilliseconds << "] "
          << msg << std::endl;
    }
  }
}

class CzPluginFactory : public Steinberg::IPluginFactory2 {
public:
  virtual Steinberg::tresult PLUGIN_API
  queryInterface(const Steinberg::TUID _iid, void **obj) override {
    if (!_iid || !obj)
      return Steinberg::kInvalidArgument;
    if (memcmp(_iid, Steinberg::FUnknown::iid, 16) == 0 ||
        memcmp(_iid, Steinberg::IPluginFactory::iid, 16) == 0 ||
        memcmp(_iid, Steinberg::IPluginFactory2::iid, 16) == 0) {
      *obj = static_cast<Steinberg::IPluginFactory2 *>(this);
      addRef();
      return Steinberg::kResultOk;
    }
    *obj = nullptr;
    return Steinberg::kNoInterface;
  }

  virtual Steinberg::uint32 PLUGIN_API addRef() override {
    return ++ref_count_;
  }
  virtual Steinberg::uint32 PLUGIN_API release() override {
    uint32_t r = --ref_count_;
    if (r == 0)
      delete this;
    return r;
  }

  virtual Steinberg::tresult PLUGIN_API
  getFactoryInfo(Steinberg::PFactoryInfo *info) override {
    LogDebug("CzPluginFactory::getFactoryInfo() called");
    if (!info)
      return Steinberg::kInvalidArgument;
    memset(info, 0, sizeof(Steinberg::PFactoryInfo));
    strncpy_s(info->vendor, "CZSYSEX", sizeof(info->vendor) - 1);
    strncpy_s(info->url, "https://github.com/revert/CZSYSEX",
              sizeof(info->url) - 1);
    strncpy_s(info->email, "support@czsysex.local", sizeof(info->email) - 1);
    info->flags = Steinberg::PFactoryInfo::kUnicode;
    return Steinberg::kResultOk;
  }

  virtual Steinberg::int32 PLUGIN_API countClasses() override { return 1; }

  virtual Steinberg::tresult PLUGIN_API getClassInfo(Steinberg::int32 index,
                                                     void *info) override {
    LogDebug("CzPluginFactory::getClassInfo() called");
    if (index != 0 || !info)
      return Steinberg::kInvalidArgument;
    struct PClassInfo {
      Steinberg::TUID cid;
      Steinberg::int32 cardinality;
      char category[32];
      char name[64];
    };
    static_assert(sizeof(PClassInfo) == 116, "PClassInfo ABI size mismatch");
    auto *ci = static_cast<PClassInfo *>(info);
    memset(ci, 0, sizeof(PClassInfo));
    memcpy(ci->cid, kCZ3000ProcessorUID, 16);
    ci->cardinality = 0x7FFFFFFF;
    strncpy_s(ci->category, "Audio Module Class", sizeof(ci->category) - 1);
    strncpy_s(ci->name, "Casio CZ-3000 SysEx VST3", sizeof(ci->name) - 1);
    return Steinberg::kResultOk;
  }

  virtual Steinberg::tresult PLUGIN_API
  getClassInfo2(Steinberg::int32 index, Steinberg::PClassInfo2 *info) override {
    LogDebug("CzPluginFactory::getClassInfo2() called");
    if (index != 0 || !info)
      return Steinberg::kInvalidArgument;
    memset(info, 0, sizeof(Steinberg::PClassInfo2));
    memcpy(info->cid, kCZ3000ProcessorUID, 16);
    info->cardinality = 0x7FFFFFFF;
    strncpy_s(info->category, "Audio Module Class", sizeof(info->category) - 1);
    strncpy_s(info->name, "Casio CZ-3000 SysEx VST3", sizeof(info->name) - 1);
    info->classFlags = 0;
    strncpy_s(info->subCategories, "Instrument|External",
              sizeof(info->subCategories) - 1);
    strncpy_s(info->vendor, "CZSYSEX", sizeof(info->vendor) - 1);
    strncpy_s(info->version, "1.0.0", sizeof(info->version) - 1);
    strncpy_s(info->sdkVersion, "VST 3.7.0", sizeof(info->sdkVersion) - 1);
    return Steinberg::kResultOk;
  }

  virtual Steinberg::tresult PLUGIN_API
  createInstance(const Steinberg::TUID cid, const Steinberg::TUID _iid,
                 void **obj) override {
    std::stringstream ss;
    ss << "createInstance: cid=";
    for (int i = 0; i < 16; ++i)
      ss << std::hex << std::setw(2) << std::setfill('0')
         << (int)(uint8_t)cid[i];
    ss << " _iid=";
    for (int i = 0; i < 16; ++i)
      ss << std::hex << std::setw(2) << std::setfill('0')
         << (int)(uint8_t)_iid[i];
    LogDebug(ss.str());

    if (!obj) {
      LogDebug("createInstance: obj is null");
      return Steinberg::kInvalidArgument;
    }
    if (memcmp(cid, kCZ3000ProcessorUID, 16) == 0 ||
        memcmp(cid, kCZ3000ControllerUID, 16) == 0) {
      try {
        LogDebug("createInstance: instantiating CzVstProcessor");
        auto *proc = new CzVstProcessor();
        Steinberg::tresult res = proc->queryInterface(_iid, obj);
        proc->release(); // Balance constructor refCount so *obj has refCount 1
        ss.str("");
        ss.clear();
        ss << "createInstance: queryInterface result=" << res
           << " obj=" << *obj;
        LogDebug(ss.str());
        return res;
      } catch (const std::exception &e) {
        LogDebug(std::string("createInstance std::exception: ") + e.what());
        return Steinberg::kInternalError;
      } catch (...) {
        LogDebug("createInstance unknown exception");
        return Steinberg::kInternalError;
      }
    }
    LogDebug("createInstance: CID mismatch");
    *obj = nullptr;
    return Steinberg::kNoInterface;
  }

private:
  std::atomic<uint32_t> ref_count_{1};
};

} // namespace cz

static cz::CzPluginFactory *g_factory = nullptr;
HINSTANCE g_hModule = nullptr;

extern "C" {

__declspec(dllexport) bool PLUGIN_API InitDll() {
  cz::LogDebug("InitDll() called");
  return true;
}

__declspec(dllexport) bool PLUGIN_API ExitDll() {
  cz::LogDebug("ExitDll() called");
  if (g_factory) {
    g_factory->release();
    g_factory = nullptr;
  }
  return true;
}

__declspec(dllexport) Steinberg::IPluginFactory *PLUGIN_API GetPluginFactory() {
  cz::LogDebug("GetPluginFactory() called");
  if (!g_factory) {
    g_factory = new cz::CzPluginFactory();
  } else {
    g_factory->addRef();
  }
  return g_factory;
}

} // extern "C"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason,
                    LPVOID /*lpvReserved*/) {
  if (fdwReason == DLL_PROCESS_ATTACH) {
    g_hModule = hinstDLL;
    DisableThreadLibraryCalls(hinstDLL);
  } else if (fdwReason == DLL_PROCESS_DETACH) {
    ExitDll();
  }
  return TRUE;
}
