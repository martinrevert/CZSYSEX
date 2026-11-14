#pragma once

#include "../core/cz_bank.h"
#include "../midi/cz_midi.h"
#include "imgui.h"
#include <string>

namespace cz {

enum class LibrarianAction {
  None = 0,
  PatchSelected,
  RequestSingleDump,
  BatchDownloadPresets,
  BatchDownloadMemories,
  RandomizeMemory
};

class CzLibrarian {
public:
  CzLibrarian(CzBank &bank, CzMidiManager &midi);

  // Renders the dual 32-Preset / 32-Memory bank view
  LibrarianAction render(BankMode &active_mode, size_t &active_patch_index,
                         uint8_t midi_channel, bool &live_audition_sync,
                         bool &auto_fetch_on_select, bool batch_active);

  const std::string &get_status_message() const { return status_msg_; }
  void set_status_message(const std::string &msg) { status_msg_ = msg; }

private:
  CzBank &bank_;
  CzMidiManager &midi_;

  std::string status_msg_ = "Librarian ready.";
  CzPatch clipboard_patch_;
  bool has_clipboard_ = false;
  int viewed_bank_ = 0; // 0=A, 1=B, 2=C, 3=D
  size_t last_synced_patch_index_ = 0;
  BankMode last_synced_mode_ = BankMode::Preset;

  char rename_buf_[32] = {0};
  int renaming_index_ = -1;
};

} // namespace cz
