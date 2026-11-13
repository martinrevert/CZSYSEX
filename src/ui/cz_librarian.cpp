#include "cz_librarian.h"
#include "../core/cz_sysex.h"
#include "cz_tutorial.h"
#include <cstdio>
#include <cstring>

namespace cz {

CzLibrarian::CzLibrarian(CzBank& bank, CzMidiManager& midi)
    : bank_(bank), midi_(midi) {
}

bool CzLibrarian::render(size_t& active_patch_index) {
    bool patch_changed = false;

    if (active_patch_index != last_synced_patch_index_) {
        viewed_bank_ = static_cast<int>(active_patch_index / 8);
        last_synced_patch_index_ = active_patch_index;
    }

    ImGui::PushID("CzLibrarianPanel");

    ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.85f, 1.0f), "GESTOR DE BANCO (32 RANURAS)");
    ImGui::TextColored(ImVec4(0.65f, 0.68f, 0.72f, 1.0f), "Estado: %s", status_msg_.c_str());

    // Toolbar actions - Row 1
    if (ImGui::SmallButton("Enviar al Búfer (Audición)")) {
        const CzPatch& current = bank_.get_patch_by_index(active_patch_index);
        // Send to recall buffer slot 0x60
        CzPatch audition = current;
        audition.memory.location = "recall";
        auto syx = CzSysEx::patch_to_sysex(audition, true);
        if (midi_.send_sysex(syx)) {
            status_msg_ = "Parche enviado al búfer de edición del CZ-3000.";
        } else {
            status_msg_ = "Error al enviar: " + midi_.get_last_error();
        }
    }
    CzTutorial::instance().tooltip("sysex_io");

    ImGui::SameLine();
    if (ImGui::SmallButton("Guardar en Memoria Synth")) {
        const CzPatch& current = bank_.get_patch_by_index(active_patch_index);
        auto syx = CzSysEx::patch_to_sysex(current, true);
        if (midi_.send_sysex(syx)) {
            char b = current.memory.bank;
            int s = current.memory.slot;
            status_msg_ = "Parche guardado en memoria interna " + std::string(1, b) + std::to_string(s) + " del CZ.";
        } else {
            status_msg_ = "Error al guardar en memoria: " + midi_.get_last_error();
        }
    }
    CzTutorial::instance().tooltip("sysex_io");

    // Toolbar actions - Row 2
    if (ImGui::SmallButton("Enviar Banco Completo (32)")) {
        std::vector<std::vector<uint8_t>> all_syx;
        for (size_t i = 0; i < CzBank::BANK_SIZE; ++i) {
            all_syx.push_back(CzSysEx::patch_to_sysex(bank_.patches[i], true));
        }
        if (midi_.send_sysex_bank(all_syx, 40)) {
            status_msg_ = "Banco completo de 32 patches transmitido al CZ-3000.";
        } else {
            status_msg_ = "Error al transmitir banco: " + midi_.get_last_error();
        }
    }
    CzTutorial::instance().tooltip("sysex_io");

    ImGui::SameLine();
    if (ImGui::SmallButton("Copiar")) {
        clipboard_patch_ = bank_.get_patch_by_index(active_patch_index);
        has_clipboard_ = true;
        status_msg_ = "Parche copiado al portapapeles.";
    }

    if (has_clipboard_) {
        ImGui::SameLine();
        if (ImGui::SmallButton("Pegar")) {
            auto [b, s] = CzBank::get_bank_slot(active_patch_index);
            bank_.patches[active_patch_index] = clipboard_patch_;
            bank_.patches[active_patch_index].memory.bank = b;
            bank_.patches[active_patch_index].memory.slot = s;
            patch_changed = true;
            status_msg_ = "Parche pegado en ranura actual.";
        }
    }

    ImGui::Separator();

    // Bank Selector (A, B, C, D)
    static const char bank_letters[] = {'A', 'B', 'C', 'D'};
    for (int b = 0; b < 4; ++b) {
        if (b > 0) ImGui::SameLine();
        bool active_bank = (viewed_bank_ == b);
        if (active_bank) {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 145, 165, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(42, 46, 52, 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(190, 195, 200, 255));
        }
        char btn_label[16];
        snprintf(btn_label, sizeof(btn_label), "Banco %c##selbank%d", bank_letters[b], b);
        if (ImGui::Button(btn_label, ImVec2(68, 22))) {
            viewed_bank_ = b;
        }
        ImGui::PopStyleColor(2);
    }

    // 8-Sound List for selected bank
    char bank_char = bank_letters[viewed_bank_];
    ImGui::BeginChild("BankSoundList", ImVec2(0, 0), true);
    for (int s = 1; s <= 8; ++s) {
        size_t idx = CzBank::get_index(bank_char, s);
        CzPatch& p = bank_.patches[idx];

        char slot_label[48];
        snprintf(slot_label, sizeof(slot_label), "%c%d: %s", bank_char, s, p.name.c_str());

        bool is_selected = (active_patch_index == idx);
        if (ImGui::Selectable(slot_label, is_selected)) {
            active_patch_index = idx;
            last_synced_patch_index_ = idx;
            patch_changed = true;
        }

        // Right click context menu to rename
        if (ImGui::BeginPopupContextItem()) {
            ImGui::Text("Renombrar Parche %c%d:", bank_char, s);
            char edit_name[32];
            strncpy_s(edit_name, p.name.c_str(), sizeof(edit_name) - 1);
            if (ImGui::InputText("##Name", edit_name, sizeof(edit_name), ImGuiInputTextFlags_EnterReturnsTrue)) {
                p.name = edit_name;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Listo")) {
                p.name = edit_name;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();

    ImGui::PopID();
    return patch_changed;
}

} // namespace cz

