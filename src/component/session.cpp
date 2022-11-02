#include "session.hpp"
#include <iostream>
bool session_t::process() {
    if (!active)
        return false;

    {
        ImGui::Begin(ImGui::GetKeyName(key), &active);
        if (ImGui::Button("Clear")) {
            records.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Paused", &paused);
        if (!paused && !funcs::IsLegacyNativeDupe(key)) {
            if (ImGui::IsKeyDown(key) && state == keystate_t::Released) {
                records.emplace_back(record_t{ keypress_type_t::Press, std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time });
                state = keystate_t::Pressed;
            }
            if (!ImGui::IsKeyDown(key) && state == keystate_t::Pressed) {
                records.emplace_back(record_t{ keypress_type_t::Release, std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - start_time });
                state = keystate_t::Released;
            }
        }

        if (ImGui::BeginTable(ImGui::GetKeyName(key), 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            for (auto& record : records) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text(record.type == keypress_type_t::Press ? "Press" : "Release");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%lf", record.timestamp * 1e-9);
            }
            ImGui::EndTable();
        }
        ImGui::End();
    }

    return true;
}