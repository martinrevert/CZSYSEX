#include "cz_midi.h"
#include <thread>
#include <chrono>

#pragma comment(lib, "winmm.lib")

namespace cz {

CzMidiManager::CzMidiManager() {
    for (size_t i = 0; i < NUM_IN_BUFFERS; ++i) {
        in_buffers_[i].resize(SYSEX_BUFFER_SIZE, 0);
    }
}

CzMidiManager::~CzMidiManager() {
    close_input();
    close_output();
}

std::vector<MidiDeviceInfo> CzMidiManager::get_input_devices() {
    std::vector<MidiDeviceInfo> devices;
    UINT num = midiInGetNumDevs();
    for (UINT i = 0; i < num; ++i) {
        MIDIINCAPSA caps{};
        if (midiInGetDevCapsA(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            devices.push_back({static_cast<int>(i), caps.szPname});
        }
    }
    return devices;
}

std::vector<MidiDeviceInfo> CzMidiManager::get_output_devices() {
    std::vector<MidiDeviceInfo> devices;
    UINT num = midiOutGetNumDevs();
    for (UINT i = 0; i < num; ++i) {
        MIDIOUTCAPSA caps{};
        if (midiOutGetDevCapsA(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            devices.push_back({static_cast<int>(i), caps.szPname});
        }
    }
    return devices;
}

void CALLBACK CzMidiManager::midi_in_proc(HMIDIIN /*hMidiIn*/, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR /*dwParam2*/) {
    auto* self = reinterpret_cast<CzMidiManager*>(dwInstance);
    if (!self) return;

    if (wMsg == MIM_LONGDATA) {
        auto* hdr = reinterpret_cast<MIDIHDR*>(dwParam1);
        if (hdr && hdr->dwBytesRecorded > 0) {
            self->handle_in_sysex(hdr);
        }
    }
}

void CzMidiManager::handle_in_sysex(MIDIHDR* hdr) {
    if (!hdr || hdr->dwBytesRecorded == 0) return;

    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(hdr->lpData);
    std::vector<uint8_t> msg(bytes, bytes + hdr->dwBytesRecorded);

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        received_sysex_queue_.push(msg);
    }

    if (sysex_callback_) {
        sysex_callback_(msg);
    }

    // Re-queue buffer for next message
    if (h_midi_in_) {
        midiInAddBuffer(h_midi_in_, hdr, sizeof(MIDIHDR));
    }
}

bool CzMidiManager::open_input(int device_id) {
    close_input();

    MMRESULT res = midiInOpen(&h_midi_in_, static_cast<UINT>(device_id),
                             reinterpret_cast<DWORD_PTR>(&CzMidiManager::midi_in_proc),
                             reinterpret_cast<DWORD_PTR>(this),
                             CALLBACK_FUNCTION);
    if (res != MMSYSERR_NOERROR) {
        last_error_ = "Failed to open MIDI input device ID: " + std::to_string(device_id);
        h_midi_in_ = nullptr;
        return false;
    }

    current_in_id_ = device_id;

    // Prepare and add input buffers
    for (size_t i = 0; i < NUM_IN_BUFFERS; ++i) {
        ZeroMemory(&in_headers_[i], sizeof(MIDIHDR));
        in_headers_[i].lpData = in_buffers_[i].data();
        in_headers_[i].dwBufferLength = static_cast<DWORD>(in_buffers_[i].size());
        in_headers_[i].dwFlags = 0;

        midiInPrepareHeader(h_midi_in_, &in_headers_[i], sizeof(MIDIHDR));
        midiInAddBuffer(h_midi_in_, &in_headers_[i], sizeof(MIDIHDR));
    }

    midiInStart(h_midi_in_);
    return true;
}

void CzMidiManager::close_input() {
    if (h_midi_in_) {
        midiInStop(h_midi_in_);
        midiInReset(h_midi_in_);

        for (size_t i = 0; i < NUM_IN_BUFFERS; ++i) {
            if (in_headers_[i].dwFlags & MHDR_PREPARED) {
                midiInUnprepareHeader(h_midi_in_, &in_headers_[i], sizeof(MIDIHDR));
            }
        }

        midiInClose(h_midi_in_);
        h_midi_in_ = nullptr;
    }
    current_in_id_ = -1;
}

bool CzMidiManager::open_output(int device_id) {
    close_output();

    MMRESULT res = midiOutOpen(&h_midi_out_, static_cast<UINT>(device_id), 0, 0, CALLBACK_NULL);
    if (res != MMSYSERR_NOERROR) {
        last_error_ = "Failed to open MIDI output device ID: " + std::to_string(device_id);
        h_midi_out_ = nullptr;
        return false;
    }

    current_out_id_ = device_id;
    return true;
}

void CzMidiManager::close_output() {
    if (h_midi_out_) {
        midiOutReset(h_midi_out_);
        midiOutClose(h_midi_out_);
        h_midi_out_ = nullptr;
    }
    current_out_id_ = -1;
}

bool CzMidiManager::send_sysex(const std::vector<uint8_t>& data) {
    if (!h_midi_out_ || data.empty()) {
        last_error_ = "MIDI output port not open or empty data.";
        return false;
    }

    MIDIHDR hdr{};
    hdr.lpData = reinterpret_cast<LPSTR>(const_cast<uint8_t*>(data.data()));
    hdr.dwBufferLength = static_cast<DWORD>(data.size());
    hdr.dwBytesRecorded = static_cast<DWORD>(data.size());
    hdr.dwFlags = 0;

    MMRESULT res = midiOutPrepareHeader(h_midi_out_, &hdr, sizeof(MIDIHDR));
    if (res != MMSYSERR_NOERROR) {
        last_error_ = "Failed to prepare MIDI header.";
        return false;
    }

    res = midiOutLongMsg(h_midi_out_, &hdr, sizeof(MIDIHDR));
    if (res != MMSYSERR_NOERROR) {
        last_error_ = "Failed to send SysEx message.";
        midiOutUnprepareHeader(h_midi_out_, &hdr, sizeof(MIDIHDR));
        return false;
    }

    // Wait for transmission to finish
    while (!(hdr.dwFlags & MHDR_DONE)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    midiOutUnprepareHeader(h_midi_out_, &hdr, sizeof(MIDIHDR));
    return true;
}

bool CzMidiManager::send_sysex_bank(const std::vector<std::vector<uint8_t>>& patches_syx, int delay_ms) {
    if (!h_midi_out_) return false;

    for (const auto& syx : patches_syx) {
        if (!send_sysex(syx)) {
            return false;
        }
        if (delay_ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        }
    }
    return true;
}

void CzMidiManager::set_sysex_callback(SysExCallback cb) {
    sysex_callback_ = std::move(cb);
}

bool CzMidiManager::poll_sysex(std::vector<uint8_t>& out_msg) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    if (received_sysex_queue_.empty()) {
        return false;
    }
    out_msg = std::move(received_sysex_queue_.front());
    received_sysex_queue_.pop();
    return true;
}

void CzMidiManager::send_note_on(uint8_t note, uint8_t velocity, uint8_t channel) {
    if (!h_midi_out_) return;
    uint8_t ch = (channel - 1) & 0x0F;
    DWORD msg = (0x90 | ch) | (static_cast<DWORD>(note & 0x7F) << 8) | (static_cast<DWORD>(velocity & 0x7F) << 16);
    midiOutShortMsg(h_midi_out_, msg);
}

void CzMidiManager::send_note_off(uint8_t note, uint8_t channel) {
    if (!h_midi_out_) return;
    uint8_t ch = (channel - 1) & 0x0F;
    DWORD msg = (0x80 | ch) | (static_cast<DWORD>(note & 0x7F) << 8);
    midiOutShortMsg(h_midi_out_, msg);
}

void CzMidiManager::send_all_notes_off(uint8_t channel) {
    if (!h_midi_out_) return;
    uint8_t ch = (channel - 1) & 0x0F;
    // Controller 123: All Notes Off
    DWORD msg = (0xB0 | ch) | (123 << 8) | (0 << 16);
    midiOutShortMsg(h_midi_out_, msg);
}

} // namespace cz

