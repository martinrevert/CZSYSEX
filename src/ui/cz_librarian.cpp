#include "cz_librarian.h"
#include "../core/cz_sysex.h"
#include "../core/cz_i18n.h"
#include "cz_tutorial.h"
#include <cstdio>
#include <cstring>

namespace cz {

CzLibrarian::CzLibrarian(CzBank& bank, CzMidiManager& midi)
    : bank_(bank), midi_(midi) {
    status_msg_ = tr("lib.ready");
}

LibrarianAction CzLibrarian::render(BankMode& active_mode, size_t& active_patch_index,
                                     uint8_t midi_channel, bool& live_audition_sync,
                                     bool& auto_fetch_on_select, bool batch_active) {
    LibrarianAction action = LibrarianAction::None;

    if (active_patch_index != last_synced_patch_index_ || active_mode != last_synced_mode_) {
        viewed_bank_ = static_cast<int>(active_patch_index / 8);
        last_synced_patch_index_ = active_patch_index;
        last_synced_mode_ = active_mode;
    }

    ImGui::PushID("CzLibrarianPanel");

    ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.85f, 1.0f), "%s", tr("lib.title"));
    ImGui::TextColored(ImVec4(0.65f, 0.68f, 0.72f, 1.0f), tr("lib.status"), status_msg_.c_str());

    // Mode Selector: PRESET BANK (32 Fixed) vs MEMORY BANK (32 User)
    bool is_preset = (active_mode == BankMode::Preset);
    if (is_preset) {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(30, 140, 95, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(42, 46, 52, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(190, 195, 200, 255));
    }
    if (ImGui::SmallButton(tr("lib.preset_tab"))) {
        active_mode = BankMode::Preset;
        last_synced_mode_ = active_mode;
        action = LibrarianAction::PatchSelected;
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    if (!is_preset) {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(190, 95, 30, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(42, 46, 52, 255));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(190, 195, 200, 255));
    }
    if (ImGui::SmallButton(tr("lib.memory_tab"))) {
        active_mode = BankMode::Memory;
        last_synced_mode_ = active_mode;
        action = LibrarianAction::PatchSelected;
    }
    ImGui::PopStyleColor(2);

    ImGui::SameLine();
    ImGui::Checkbox(tr("lib.auto_fetch"), &auto_fetch_on_select);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tr("lib.auto_fetch_tip"));
    }

    // Toolbar actions
    if (is_preset) {
        if (ImGui::SmallButton(tr("lib.copy_to_mem"))) {
            auto [b, s] = CzBank::get_bank_slot(active_patch_index);
            CzPatch copy = bank_.preset_patches[active_patch_index];
            copy.memory.location = "internal";
            copy.memory.bank = b;
            copy.memory.slot = s;
            copy.midi_channel = midi_channel;
            bank_.patches[active_patch_index] = copy;
            active_mode = BankMode::Memory;
            last_synced_mode_ = active_mode;
            action = LibrarianAction::PatchSelected;
            status_msg_ = "Preset -> MEMORY " + std::string(1, b) + "-" + std::to_string(s);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(tr("lib.read_preset"))) {
            action = LibrarianAction::RequestSingleDump;
        }
        ImGui::SameLine();
        if (batch_active) {
            if (ImGui::SmallButton(tr("lib.cancel_dl"))) {
                action = LibrarianAction::CancelBatch;
            }
        } else {
            if (ImGui::SmallButton(tr("lib.dl_32_presets"))) {
                action = LibrarianAction::BatchDownloadPresets;
            }
        }
        ImGui::SameLine();
        ImGui::TextColored(bank_.is_presets_cached() ? ImVec4(0.3f, 0.9f, 0.5f, 1.0f)
                                                     : ImVec4(0.8f, 0.7f, 0.3f, 1.0f),
                           "%s", bank_.is_presets_cached() ? tr("lib.cache_ok") : tr("lib.no_cache"));
    } else {
        if (ImGui::SmallButton(tr("lib.send_buf"))) {
            CzPatch audition = bank_.patches[active_patch_index];
            audition.midi_channel = midi_channel;
            audition.memory.location = "recall";
            auto syx = CzSysEx::patch_to_sysex(audition, true);
            if (midi_.send_sysex(syx)) {
                status_msg_ = "Sent to COMPARE/RECALL (0x60).";
            } else {
                status_msg_ = midi_.get_last_error();
            }
        }
        CzTutorial::instance().tooltip("sysex_io");

        ImGui::SameLine();
        if (ImGui::SmallButton(tr("lib.save_synth"))) {
            CzPatch target = bank_.patches[active_patch_index];
            target.midi_channel = midi_channel;
            target.memory.location = "internal";
            auto syx = CzSysEx::patch_to_sysex(target, true);
            if (midi_.send_sysex(syx)) {
                char b = target.memory.bank;
                int s = target.memory.slot;
                status_msg_ = "Written to MEMORY " + std::string(1, b) + "-" + std::to_string(s);
            } else {
                status_msg_ = midi_.get_last_error();
            }
        }
        CzTutorial::instance().tooltip("sysex_io");

        ImGui::SameLine();
        if (ImGui::SmallButton(tr("lib.read_synth"))) {
            action = LibrarianAction::RequestSingleDump;
        }

        ImGui::SameLine();
        if (batch_active) {
            if (ImGui::SmallButton(tr("lib.cancel_dl"))) {
                action = LibrarianAction::CancelBatch;
            }
        } else {
            if (ImGui::SmallButton(tr("lib.dl_32_mem"))) {
                action = LibrarianAction::BatchDownloadMemories;
            }
        }

        if (ImGui::SmallButton(tr("lib.send_bank"))) {
            std::vector<std::vector<uint8_t>> all_syx;
            for (size_t i = 0; i < CzBank::BANK_SIZE; ++i) {
                CzPatch p = bank_.patches[i];
                p.midi_channel = midi_channel;
                p.memory.location = "internal";
                all_syx.push_back(CzSysEx::patch_to_sysex(p, true));
            }
            if (midi_.send_sysex_bank(all_syx, 40)) {
                status_msg_ = "32-patch bank sent to CZ-3000.";
            } else {
                status_msg_ = midi_.get_last_error();
            }
        }
        CzTutorial::instance().tooltip("sysex_io");

        ImGui::SameLine();
        if (ImGui::SmallButton(tr("lib.copy"))) {
            clipboard_patch_ = bank_.patches[active_patch_index];
            has_clipboard_ = true;
            status_msg_ = "Patch copied.";
        }

        if (has_clipboard_) {
            ImGui::SameLine();
            if (ImGui::SmallButton(tr("lib.paste"))) {
                auto [b, s] = CzBank::get_bank_slot(active_patch_index);
                bank_.patches[active_patch_index] = clipboard_patch_;
                bank_.patches[active_patch_index].memory.location = "internal";
                bank_.patches[active_patch_index].memory.bank = b;
                bank_.patches[active_patch_index].memory.slot = s;
                action = LibrarianAction::PatchSelected;
                status_msg_ = "Pasted to MEMORY " + std::string(1, b) + "-" + std::to_string(s);
            }
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(130, 65, 175, 255));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(155, 85, 205, 255));
        if (ImGui::SmallButton(tr("lib.randomize"))) {
            auto [b, s] = CzBank::get_bank_slot(active_patch_index);
            CzPatch& target = bank_.patches[active_patch_index];
            target.randomize();
            target.memory.location = "internal";
            target.memory.bank = b;
            target.memory.slot = s;
            target.midi_channel = midi_channel;
            action = LibrarianAction::RandomizeMemory;
            status_msg_ = "Randomized MEMORY " + std::string(1, b) + "-" + std::to_string(s);
        }
        ImGui::PopStyleColor(2);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tr("lib.randomize_tip"));
        }

        ImGui::SameLine();
        ImGui::Checkbox(tr("lib.sync_60"), &live_audition_sync);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", tr("lib.sync_60_tip"));
        }
    }

    ImGui::Separator();

    // Bank Selector (A, B, C, D)
    static const char bank_letters[] = {'A', 'B', 'C', 'D'};
    for (int b = 0; b < 4; ++b) {
        if (b > 0) ImGui::SameLine();
        bool active_bank = (viewed_bank_ == b);
        if (active_bank) {
            ImGui::PushStyleColor(ImGuiCol_Button, is_preset ? IM_COL32(30, 140, 95, 255) : IM_COL32(0, 145, 165, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(42, 46, 52, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(190, 195, 200, 255));
        }
        char btn_label[32];
        snprintf(btn_label, sizeof(btn_label), tr("lib.bank_btn"), bank_letters[b], b);
        if (ImGui::Button(btn_label, ImVec2(72, 22))) {
            viewed_bank_ = b;
        }
        ImGui::PopStyleColor(2);
    }

    // 8-Sound List for selected bank
    char bank_char = bank_letters[viewed_bank_];
    ImGui::BeginChild("BankSoundList", ImVec2(0, 0), true);
    for (int s = 1; s <= 8; ++s) {
        size_t idx = CzBank::get_index(bank_char, s);
        CzPatch& p = is_preset ? bank_.preset_patches[idx] : bank_.patches[idx];

        char slot_label[64];
        if (is_preset && p.line_select == LineSelect::Line1) {
            snprintf(slot_label, sizeof(slot_label), "%c-%d: %s * (1-DCO)", bank_char, s, p.name.c_str());
        } else {
            snprintf(slot_label, sizeof(slot_label), "%c-%d: %s", bank_char, s, p.name.c_str());
        }

        bool is_selected = (active_patch_index == idx);
        if (ImGui::Selectable(slot_label, is_selected)) {
            active_patch_index = idx;
            last_synced_patch_index_ = idx;
            action = LibrarianAction::PatchSelected;
        }

        // Right click context menu to rename (Memory mode only)
        if (!is_preset && ImGui::BeginPopupContextItem()) {
            ImGui::Text(tr("lib.rename"), bank_char, s);
            char edit_name[32];
            strncpy_s(edit_name, p.name.c_str(), sizeof(edit_name) - 1);
            if (ImGui::InputText("##Name", edit_name, sizeof(edit_name), ImGuiInputTextFlags_EnterReturnsTrue)) {
                p.name = edit_name;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button(tr("lib.done"))) {
                p.name = edit_name;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();

    ImGui::PopID();
    return action;
}

} // namespace cz

