#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include <windows.h>
#include <mmsystem.h>

namespace cz {

struct MidiDeviceInfo {
  int id = -1;
  std::string name;
};

class CzMidiManager {
public:
  using SysExCallback = std::function<void(const std::vector<uint8_t> &)>;

  CzMidiManager();
  ~CzMidiManager();

  // Port enumeration
  std::vector<MidiDeviceInfo> get_input_devices();
  std::vector<MidiDeviceInfo> get_output_devices();

  // Connection
  bool open_input(int device_id);
  void close_input();
  bool is_input_open() const { return h_midi_in_ != nullptr; }
  int current_input_id() const { return current_in_id_; }

  bool open_output(int device_id);
  void close_output();
  bool is_output_open() const { return h_midi_out_ != nullptr; }
  int current_output_id() const { return current_out_id_; }

  // SysEx Send & Receive
  bool send_sysex(const std::vector<uint8_t> &data);
  bool send_sysex_bank(const std::vector<std::vector<uint8_t>> &patches_syx,
                       int delay_ms = 40);

  void set_sysex_callback(SysExCallback cb);
  bool poll_sysex(std::vector<uint8_t> &out_msg);

  // Note audition & Program Change
  void send_note_on(uint8_t note, uint8_t velocity = 100, uint8_t channel = 1);
  void send_note_off(uint8_t note, uint8_t channel = 1);
  void send_all_notes_off(uint8_t channel = 1);
  void send_program_change(uint8_t program, uint8_t channel = 1);

  // Bidirectional SysEx request (SEND REQUEST 1)
  bool request_patch_dump(uint8_t midi_channel, uint8_t slot_byte);

  // Status & Error
  const std::string &get_last_error() const { return last_error_; }

private:
  static void CALLBACK midi_in_proc(HMIDIIN hMidiIn, UINT wMsg,
                                    DWORD_PTR dwInstance, DWORD_PTR dwParam1,
                                    DWORD_PTR dwParam2);
  void handle_in_sysex(MIDIHDR *hdr);

  HMIDIIN h_midi_in_ = nullptr;
  HMIDIOUT h_midi_out_ = nullptr;

  int current_in_id_ = -1;
  int current_out_id_ = -1;

  static constexpr size_t SYSEX_BUFFER_SIZE = 1024;
  static constexpr size_t NUM_IN_BUFFERS = 4;
  std::array<MIDIHDR, NUM_IN_BUFFERS> in_headers_{};
  std::array<std::vector<char>, NUM_IN_BUFFERS> in_buffers_{};

  std::mutex queue_mutex_;
  std::queue<std::vector<uint8_t>> received_sysex_queue_;
  std::vector<uint8_t> in_sysex_accum_;
  SysExCallback sysex_callback_;

  std::string last_error_;
};

} // namespace cz